#pragma once
#include <memory>
class ModelTransform;
class ViewTransform;
class ProjectionTransform;
class ViewportTransform;

class RenderPipline {
public:
	struct RenderHyperConfig
	{
	}
public:
	explicit RenderPipline(RenderHyperConfig& config);

private:
	std::unique_ptr<ModelTransform>

};