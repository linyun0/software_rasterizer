# MyTinyRender：CPU + OpenGL 双模式开发计划

> 文档日期：2026-08-18
> 目标：在现有纯 CPU 软件光栅化器基础上，引入 OpenGL 3.3 Core 渲染后端，实现 CPU / OpenGL 双模式切换，两种模式渲染同一 3D 模型并尽量保持视觉一致。

---

## 1. 当前项目状态

- 纯 CPU 软件光栅化器（GAMES101 风格），已用 Qt6 编译通过。
- 渲染路径：`DrawAreaWidget` 直接持有 `RasterizationDevice` + `MVPTransformer` + `TextureImage`。
- `Shaders/default.vert` / `default.frag` 已存在（Blinn-Phong，`#version 330 core`），但**没有任何 C++ 代码引用**（孤立资产）。
- `Camera` 类是**空壳**：`RotateAroundTarget` / `MoveTarget` / `CloseToTarget` / `SetModel` 方法体为空，且从未被使用。
- `MVPTransformer`（Eigen）才是 CPU 路径实际使用的相机，`SetModelArc` 为**增量式**旋转。
- `geometry.h` 中的 `Mesh` 已带 `m_indices`（索引）和完整 `Vertex`（Position/Normal/TexCoords），可直接作为 OpenGL 输入。

---

## 2. 总体架构设计

```
main.cpp（设 QSurfaceFormat 3.3 Core）
  └── TinyRenderMainWindow
        ├── DrawAreaWidget（★ 容器 + 协调者）
        │     ├── QStackedLayout
        │     │     ├── 页0：CpuRenderView（QPainter 画 QImage）
        │     │     └── 页1：GlRenderWidget（QOpenGLWidget）
        │     ├── Camera（唯一相机，glm + Eigen 双输出）
        │     ├── Model（共享模型数据，Assimp 加载一次）
        │     └── TextureImage（共享纹理，CPU 用 QImage / GL 转 QOpenGLTexture）
        └── ControlDockWidget（"Rotation" + "切换模式"按钮）
```

### 关键设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 渲染器切换 | `QStackedLayout` 容器化 | CPU（QPainter）与 OpenGL（paintGL）显示机制不同，不强行用统一接口 |
| 相机 | 重写 `Camera` 为唯一相机 | 消除 `MVPTransformer` 与空壳 `Camera` 的重复/割裂 |
| 着色器加载 | Qt 资源 `.qrc` | 编译进 exe，部署无额外文件 |
| 光照参数 | 抽 `SceneLighting` 单一来源 | 保证 CPU 与 OpenGL 视觉一致 |
| 模型数据 | 共享 `Model`；CPU 用 `Triangle[]`，GL 用 `Mesh` 索引 | 各取所需，GL 更高效 |

---

## 3. 核心接口设计（接口级）

### 3.1 Camera（重写 `TinyRenderSrc/Camera.h/.cpp`）

内部用 **glm** 存矩阵（轨道数学都在 glm 里做），同时提供两套 getter 满足两类消费者。

```cpp
class Camera {
public:
    Camera();

    // ---- 透视参数 ----
    void SetPerspective(float fov, float aspect, float near, float far);

    // ---- 轨道交互（供 OpenGL 鼠标事件调用）----
    void Orbit(float deltaX, float deltaY);   // 左键拖动：球坐标绕目标旋转
    void Zoom(float delta);                   // 滚轮：沿视线改变半径
    void Pan(float deltaX, float deltaY);     // 中键拖动：平移目标点

    // ---- 模型变换（绝对角度，非增量）----
    void SetModelAngle(float angle);          // 0~360 环绕 Y 轴
    void SetModelScale(float scale);
    void SetModelTransform(float angle, float scale);

    // ---- 相机位置 ----
    void SetPosition(const glm::vec3& pos);
    void SetTarget(const glm::vec3& target);
    glm::vec3 GetPosition() const;
    glm::vec3 GetTarget() const;

    // ---- glm 输出（供 OpenGL / GlRenderWidget）----
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetModelMatrix();

    // ---- Eigen 输出（供 CPU / RasterizationDevice，复刻 MVPTransformer 接口）----
    Eigen::Matrix4f get_view_matrix();
    Eigen::Matrix4f get_projection_matrix();
    Eigen::Matrix4f get_model_matrix();

    float GetModelAngle() const;
    float GetModelScale() const;

private:
    void UpdateViewMatrix();      // glm::lookAt
    void UpdateProjectionMatrix();// glm::perspective
    void UpdateModelMatrix();     // 绕Y旋转 * 缩放
    void SyncSphericalFromPosition();
    static Eigen::Matrix4f ToEigen(const glm::mat4& m);

    glm::vec3 m_position{0,0,10};
    glm::vec3 m_target{0,0,0};
    glm::vec3 m_up{0,1,0};

    float m_fov=45, m_aspect=1, m_zNear=0.1, m_zFar=50;
    float m_theta=0, m_phi=0, m_radius=10;   // 球坐标（orbit 用）
    float m_modelAngle=140, m_modelScale=2;  // 与现有 MVPTransformer 初始值一致

    glm::mat4 m_view{1}, m_proj{1}, m_model{1};
    bool m_viewDirty=true, m_projDirty=true, m_modelDirty=true;
};
```

**关键设计点：**
- 小写 `get_view_matrix()` 等返回 **Eigen**，是为了让 `RasterizationDevice::Draw()` 里 `m_transformer->get_view_matrix()` 这行调用**零改动**——只需把 `m_transformer` 类型从 `MVPTransformer*` 改成 `Camera*`。
- 大写 `GetViewMatrix()` 等返回 **glm**，供 `GlRenderWidget` 设 uniform。
- getter 内做**懒更新**（dirty 检查），避免每次取矩阵都重算。
- **球坐标**存 `(theta, phi, radius)`，`Orbit` 只改角度、`Zoom` 只改半径，这样旋转/缩放互不干扰。

### 3.2 SceneLighting（新建 `TinyRenderSrc/SceneLighting.h`）

```cpp
struct SceneLighting {
    glm::vec3 lightPositions[2]   = { {20,20,20}, {-20,20,0} };
    glm::vec3 lightIntensities[2] = { {500,500,500}, {500,500,500} };
    glm::vec3 ambientIntensity    = {10,10,10};
    glm::vec3 eyePos              = {0,0,10};
    float ka = 0.005f;
    float ks = 0.7937f;
    float specularPower = 150.0f;
};
```

- CPU 端 `texture_fragment_shader` 从该结构体读参数（替换当前硬编码）。
- OpenGL 端 `paintGL` 把该结构体字段设成 uniform。

### 3.3 GlRenderWidget（新建 `TinyRenderSrc/GlRenderWidget.h/.cpp`）

```cpp
class GlRenderWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit GlRenderWidget(QWidget* parent = nullptr);
    ~GlRenderWidget();

    void SetCamera(Camera* camera);
    void SetModel(Model* model);
    void SetTexture(TextureImage* tex);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;

private:
    struct GlMesh { GLuint vao, vbo, ebo; int indexCount; };

    void SetupModel();      // Mesh 数据 → VBO/VAO/EBO
    void SetupTexture();    // QImage → QOpenGLTexture
    void CleanupGL();
    bool LoadShaders();     // 从 qrc 加载 default.vert / default.frag

    QOpenGLShaderProgram* m_program = nullptr;
    QOpenGLTexture* m_texture = nullptr;
    std::vector<GlMesh> m_meshes;

    Camera* m_camera = nullptr;          // 不拥有
    Model* m_model = nullptr;            // 不拥有
    TextureImage* m_textureImage = nullptr; // 不拥有

    QPoint m_lastMousePos;
    bool m_leftDown = false, m_middleDown = false;
    bool m_glInitialized = false;
};
```

**顶点数据布局（交错，stride = 8 × float）：**
```
location 0: aPos    (3 float, offset 0)
location 1: aNormal (3 float, offset 12)
location 2: aTexCoord (2 float, offset 24)
```

**鼠标交互映射：**
| 操作 | 事件 | Camera 调用 |
|------|------|-------------|
| 左键拖动 | mouseMoveEvent | `Orbit(dx*0.005, dy*0.005)` |
| 滚轮 | wheelEvent | `Zoom(delta*1.5)` |
| 中键拖动 | mouseMoveEvent | `Pan(dx, dy)` |

### 3.4 DrawAreaWidget（修改 `TinyRenderUI/DrawAreaWidget.h/.cpp`）

```cpp
class DrawAreaWidget : public QWidget {
    Q_OBJECT
public:
    enum RenderMode { CPU, OpenGL };
    DrawAreaWidget(QWidget* parent);
    ~DrawAreaWidget();

    void SwitchRenderMode(RenderMode mode);
    RenderMode GetMode() const;
    void SetModeArc(float angle, float scale);  // 保留，供 "Rotation" 按钮

protected:
    void resizeEvent(QResizeEvent*) override;
    void showEvent(QShowEvent*) override;

private:
    void CreateCpuRenderer();
    void CreateGlRenderer();

    Camera* m_camera = nullptr;
    Model* m_model = nullptr;
    TextureImage* m_textureImage = nullptr;

    RasterizationDevice* m_device = nullptr;   // CPU 后端
    GlRenderWidget* m_glRenderer = nullptr;    // OpenGL 后端

    QStackedLayout* m_stack = nullptr;
    QWidget* m_cpuView = nullptr;              // 页0：QPainter 画 QImage

    std::vector<Triangle*> m_triangles;        // CPU 专用
    QImage* m_showImage = nullptr;             // CPU 专用

    RenderMode m_mode = CPU;
};
```

**切换逻辑：**
- `SwitchRenderMode(OpenGL)`：懒创建 `GlRenderWidget` → 设相机/模型/纹理 → `m_stack->setCurrentIndex(1)`。
- `SwitchRenderMode(CPU)`：`m_stack->setCurrentIndex(0)`，若 CPU 画面脏则重绘。

---

## 4. 数据流

```
模型加载（一次）：Model(Assimp) → Mesh[]（vertices + indices）

CPU 路径：
  Mesh.vertices → 每 3 个造 Triangle[] → RasterizationDevice.Draw()
  → MVP 变换 → 光栅化 → z-buffer → Blinn-Phong → QImage → QPainter

OpenGL 路径：
  Mesh.vertices + indices → VBO/VAO/EBO → GlRenderWidget.paintGL()
  → 顶点/片元着色器 → Blinn-Phong → 屏幕
```

共享：`Camera`（矩阵）、`Model`（模型）、`SceneLighting`（光照）、纹理源 `QImage`。

---

## 5. 7 天详细任务清单

### Day 1 — 环境与构建基础

**目标**：OpenGL 能用的地基，不动渲染逻辑。

**任务**
- [ ] `CMakeLists.txt`：`find_package(Qt6 COMPONENTS ... OpenGL OpenGLWidgets)`，链接 `Qt6::OpenGL`
- [ ] 新建 `TinyRenderSrc/Shaders/shaders.qrc`，注册 `default.vert` / `default.frag`
- [ ] `main.cpp`：`QSurfaceFormat` 设 3.3 Core + 24 深度，`setDefaultFormat`
- [ ] 确认 `file(GLOB)` 能覆盖新增 `.cpp/.h`

**涉及文件**：`CMakeLists.txt`、`main.cpp`、`Shaders/shaders.qrc`（新）

**验收**：配置通过 → 编译通过 → 程序启动，CPU 模式照常显示模型。

---

### Day 2 — 统一相机（重写 Camera）

**目标**：`Camera` 成为唯一相机源，矩阵与当前 `MVPTransformer` 输出一致。

**任务**
- [ ] 按 3.1 接口实现 `Camera` 全部方法
- [ ] 实现 `ToEigen(glm::mat4)` 转换（注意 glm 列主序 → Eigen 逐元素拷贝）
- [ ] 实现懒更新（三个 dirty 标志）
- [ ] `Orbit` 中 clamp 仰角 `phi ∈ [-89°, 89°]` 防翻转

**验收**：写临时打印对比 `Camera::get_view_matrix()` 与 `MVPTransformer::get_view_matrix()`，数值一致（初始 eye `(0,0,10)`、lookAt `(0,0,0)`、模型绕 Y 140° + 缩放 2）。

**易踩坑**：`SetModelArc` 是增量，新 `Camera` 是绝对角度，语义对齐见 D3。

---

### Day 3 — CPU 路径迁移 + 回归

**目标**：CPU 走 `Camera`，渲染结果不变。

**任务**
- [ ] `RasterizationDevice.h`：`MVPTransformer* m_transformer` → `Camera* m_transformer`
- [ ] `RasterizationDevice.cpp`：include 换成 `Camera.h`（`Draw()` 里 `get_*_matrix()` 调用不变）
- [ ] `DrawAreaWidget.cpp`：`new MVPTransformer()` → `new Camera()`；`SetModeArc` 改为调 `Camera::SetModelAngle(绝对角度)`
- [ ] `MVPTransformer` 可删除，或暂时保留但不再被引用

**验收**：**截图对比**改造前后 CPU 渲染结果一致。这是全计划最重要的回归关卡。

**易踩坑**：`Camera::SetModelAngle` 是绝对角度，`DrawAreaWidget::SetModeArc(angle)` 需先 `GetModelAngle()` 再叠加，避免语义错位。

---

### Day 4 — GlRenderWidget 静态渲染（最重）

**目标**：OpenGL 渲染出带纹理模型，先不做交互。

**任务（按顺序）**
- [ ] `initializeGL`：`initializeOpenGLFunctions()` → `glEnable(GL_DEPTH_TEST)` + `glEnable(GL_CULL_FACE)` + `glClearColor`
- [ ] `LoadShaders`：从 qrc 用 `addShaderFromSourceFile` 加载，检查 `log()`
- [ ] `SetupModel`：遍历 `Model::meshes`，交错顶点上传 VBO，索引上传 EBO，配 VAO 三个 attribute
- [ ] `SetupTexture`：`TextureImage` 的 `QImage` → `QOpenGLTexture`（`mirrored()` 处理 UV 方向）
- [ ] `paintGL`：`glClear` → bind program → 设 uniform（MVP、光照、`viewPos`、`uTexture`）→ 每 mesh `glDrawElements`
- [ ] 临时在窗口显示 `GlRenderWidget`，硬编码相机验证

**涉及文件**：`GlRenderWidget.h/.cpp`（新）、`Shaders/default.vert`/`default.frag`（如光照需微调）

**验收**：硬编码相机下，OpenGL 显示带纹理的 Spot 模型。

**易踩坑**：
- glm 列主序 ↔ `QMatrix4x4`（`setUniformValue` 前先明确转置）
- 纹理 UV 方向（Assimp 已 `aiProcess_FlipUVs`，可能需 `mirrored`）
- 缺 `Qt6::OpenGL` 时 `QOpenGLFunctions` 未定义

---

### Day 5 — 双模式切换集成

**目标**：按钮能切换 CPU ↔ OpenGL。

**任务**
- [ ] `DrawAreaWidget` 容器化：`QStackedLayout`，页0 = `m_cpuView`，页1 = `m_glRenderer`
- [ ] `SwitchRenderMode`：懒创建 + 切换 index
- [ ] `ControlDockWidget.cpp`：加 `QPushButton("切换模式")`，`setObjectName("RendererModePushButton")`
- [ ] `DlgControl.h`：加 `#define TinyRender_RendererMode_Toggle 6`
- [ ] `ControlDockWidgetCmd.cpp`：`Activate` 加 `case TinyRender_RendererMode_Toggle` 调 `SwitchRenderMode`

**验收**：点按钮 CPU ↔ OpenGL 往返切换，两侧都显示模型。

**易踩坑**：`GlRenderWidget` 首次 `show` 后才触发 `initializeGL`，切换前确保相机/模型已注入；用 `show()/hide()` 而非 delete。

---

### Day 6 — 交互 + 光照对齐 + 一致性

**目标**：OpenGL 可交互，两模式视觉一致。

**任务**
- [ ] `GlRenderWidget` 实现 4 个鼠标事件（见 3.3 映射表），委托 `Camera`
- [ ] 新建 `SceneLighting.h`，`RasterizationDevice.cpp` 的 `texture_fragment_shader` 改从它读参数
- [ ] `GlRenderWidget::paintGL` 用 `SceneLighting` 字段设 uniform
- [ ] 同角度对比两模式，微调至肉眼一致

**验收**：OpenGL 模式流畅交互；固定角度下 CPU/OpenGL 结果肉眼一致。

---

### Day 7 — 测试与收尾

**目标**：稳定 + 交付。

**任务**
- [ ] 往返切换、快速连点切换、窗口 resize（含缩到很小）
- [ ] OpenGL 资源析构：`makeCurrent()` 后正确 `glDelete*`，无泄漏
- [ ] 边界：`GlRenderWidget` 未初始化时切换、纹理加载失败
- [ ] 清理调试输出、统一命名、更新 `README.md`

**验收**：无崩溃、无泄漏、双模式稳定。

---

## 6. 风险与应对

| 风险 | 等级 | 应对 |
|------|------|------|
| 相机统一引入矩阵/坐标系偏差 | 高 | D3 截图回归，早期暴露 |
| glm ↔ QMatrix4x4 主序错误 | 中 | D4 用单个三角形验证 MVP |
| 光照参数两处漂移 | 中 | `SceneLighting` 单一来源 |
| OpenGL 资源生命周期 | 中 | 资源在 `initializeGL` 建、析构先 `makeCurrent` |
| CPU 光栅化慢 | 低 | 预期行为，OpenGL 模式正是解法 |

---

## 7. 每日收尾习惯

- 每天结束 `git add -A && git commit`，提交信息带日期和当日里程碑。
- 在文档末尾记录"今日完成 / 遗留问题"，次日开工先回顾。

---

## 8. 完成验收清单（最终）

- [ ] Qt6 + OpenGL 编译通过，无警告
- [ ] CPU 模式渲染与改造前一致（截图回归）
- [ ] OpenGL 模式显示带纹理模型
- [ ] 按钮切换 CPU ↔ OpenGL 往返稳定
- [ ] OpenGL 鼠标 orbit / zoom / pan 流畅
- [ ] 同角度两模式视觉一致
- [ ] 无崩溃、无内存泄漏
