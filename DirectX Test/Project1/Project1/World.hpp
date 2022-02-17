#pragma once
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include "SpriteNode.h"

class World
{
public:
	explicit World(Game* Window);
	void update(const GameTimer& gt);
	void draw();

	void loadTextures(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CommandList,
		std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures);
	void buildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials);
	void buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CommandList,
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries);
	void buildScene();

public:
	enum RenderLayer
	{
		Opaque,
		Transparent,
		Count
	};

private:
	Game* mGame;
	SceneNode* mSceneGraph;
	std::array<SceneNode*, Count> mSceneLayers;
	Aircraft* mPlayerAircraft;
	SpriteNode* mBackground;
	XMFLOAT4 mWorldBounds;
	XMFLOAT2 mSpawnPosition;
	float mScrollSpeed;
};