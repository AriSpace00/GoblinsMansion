#pragma once
#include <directxtk/SimpleMath.h>

#include "DynamicText.h"
#include "Sprite2D.h"
#include "ISingleton.h"
#include "Vector3D.h"

struct Light;
class Renderer;
class ResourceManager;

class RenderManager : public ISingleton<RenderManager>
{
public:
	RenderManager() = default;
	~RenderManager() override = default;

public:
	void Initialize(HWND* hwnd,UINT width,UINT height);
	void UnInitialize();


	void Update();

	void RenderBegin();
	void GameAppRender();
	void EditorRender();
	void RenderEnd();

	bool Culling(DirectX::BoundingBox boundingBox);

	void CreateModel(std::string filename, DirectX::BoundingBox& boundingBox);

	void AddStaticMesh(const std::string& fileName, DirectX::SimpleMath::Matrix worldTM) const;


	void AddOutlineModel(const std::string& fileName, DirectX::SimpleMath::Matrix worldTM) const;

	void AddColliderBox(const Vector3D center, const Vector3D extents, const Vector3D rotation);
	void AddBoundingBox(DirectX::BoundingBox boundingBox);


	void SetBasePath(std::string filePath);

	void SetCamera(const DirectX::SimpleMath::Matrix matrix);
	
	void AddText(int entID, const std::string& text, const Vector3D& pos);
	void AddSprite(World* world, int entID, const std::string& filePath, POINT pos, float layer);
	void AddDynamicText(int entID, const std::vector<std::wstring>& textVector, int fontIndex);
	void CreatePointLight(int entID, Vector3 pos, Vector3D color, float intensity, float radius);

	void EditText(int entID, const std::string& text, const Vector3D& pos);
	void EditSprite(int entID, Sprite2D& sprite2D);
	void EditDynamicText(int size, int textIndex, int fontIndex, bool enable);
	void EditPointLight(int entID, Vector3 pos, Vector3D color, float intensity, float radius);
	void EditDirectionalLight(Vector3 dir, Vector3 color);

	void ChangeLight(int entID, Light* light);

	Renderer* GetRender() { return m_Renderer; }
	void DeleteText(int entID);
	void DeleteSprite(int entID);
	void DeleteDynamicText(int entID);
	void DeletePointLight(int entID);

private:
	Renderer* m_Renderer;
	ResourceManager* m_ResourceManager;
};

