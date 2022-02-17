#include "World.hpp"

World::World(Game* game)
	: mSceneGraph(new SceneNode(game))
	, mGame(game)
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-1.5f, 1.5, 200.0f, 0.0f)
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

void World::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);
}

void World::draw()
{
	mSceneGraph->draw();
}

void World::loadTextures(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CommandList, std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures)
{
	std::vector<std::string> texNames =
	{
		"Eagle",
		"Raptor",
		"Desert"
	};

	std::vector<std::wstring> texFilenames =
	{
		L"../../Textures/Eagle.dds",
		L"../../Textures/Raptor.dds",
		L"../../Textures/Desert.dds"
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		auto texMap = std::make_unique<Texture>();
		texMap->Name = texNames[i];
		texMap->Filename = texFilenames[i];
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
			CommandList.Get(), texMap->Filename.c_str(),
			texMap->Resource, texMap->UploadHeap));

		GameTextures[texMap->Name] = std::move(texMap);
	}
}

void World::buildMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials)
{
	auto eagle = std::make_unique<Material>();
	eagle->Name = "Eagle";
	eagle->MatCBIndex = 0;
	//! We add an index to our material definition, which references an SRV in the descriptor
	//! heap specifying the texture associated with the material :
	eagle->DiffuseSrvHeapIndex = 0;
	eagle->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	eagle->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	eagle->Roughness = 0.2f;

	auto raptor = std::make_unique<Material>();
	raptor->Name = "Raptor";
	raptor->MatCBIndex = 1;
	raptor->DiffuseSrvHeapIndex = 1;
	raptor->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	raptor->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	raptor->Roughness = 0.2f;

	auto desert = std::make_unique<Material>();
	desert->Name = "Desert";
	desert->MatCBIndex = 2;
	desert->DiffuseSrvHeapIndex = 2;
	desert->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	desert->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	desert->Roughness = 0.2f;

	GameMaterials["Eagle"] = std::move(eagle);
	GameMaterials["Raptor"] = std::move(raptor);
	GameMaterials["Desert"] = std::move(desert);
}

void World::buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CommandList, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(0.0f, 10.0f, 10.0f, 3);

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;


	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		CommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		CommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;

	GameGeometries[geo->Name] = std::move(geo);
}

void World::buildScene()
{
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mGame));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 10.0, 0.0);
	mPlayerAircraft->setScale(3.0, 3.0, 3.0);
	//mPlayerAircraft->setVeloctiy(mScrollSpeed, 0.0, 0.0);
	mSceneGraph->attachChild(std::move(player));

	std::unique_ptr<Aircraft> enemy1(new Aircraft(Aircraft::Raptor, mGame));
	auto raptor = enemy1.get();
	raptor->setPosition(0.5, 0, 1);
	raptor->setScale(1.0, 1.0, 1.0);
	raptor->setWorldRotation(0, XM_PI, 0);
	mSceneGraph->attachChild(std::move(enemy1));

	std::unique_ptr<Aircraft> enemy2(new Aircraft(Aircraft::Raptor, mGame));
	auto raptor2 = enemy2.get();
	raptor2->setPosition(0.5, 0, 1);
	raptor2->setScale(1.0, 1.0, 1.0);
	raptor2->setWorldRotation(0, XM_PI, 0);
	mSceneGraph->attachChild(std::move(enemy2));

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mGame));
	mBackground = backgroundSprite.get();
	mBackground->setPosition(1.0, 6.0, 6.0);
	mBackground->setScale(10.0, 1.0, 200.0);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	mSceneGraph->build();
}
