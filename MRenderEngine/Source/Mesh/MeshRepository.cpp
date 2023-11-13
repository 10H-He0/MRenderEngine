#include "MeshRepository.h"

MMeshRepository::MMeshRepository()
{
	FbxLoader = std::make_unique<TFbxLoader>();
	FbxLoader->Init();
}

MMeshRepository& MMeshRepository::Get()
{
	static MMeshRepository Instance;
	return Instance;
}

void MMeshRepository::Load()
{
	MMesh BoxMesh;
	BoxMesh.CreateBox(1.0f, 1.0f, 1.0f, 3);
	BoxMesh.MeshName = "BoxMesh";
	BoxMesh.GenerateBoundingBox();
	MeshMap.emplace("BoxMesh", std::move(BoxMesh));

	MMesh SphereMesh;
	SphereMesh.CreateSphere(0.5f, 20, 20);
	SphereMesh.MeshName = "SphereMesh";
	SphereMesh.GenerateBoundingBox();
	MeshMap.emplace("SphereMesh", std::move(SphereMesh));

	MMesh CylinderMesh;
	CylinderMesh.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	CylinderMesh.MeshName = "CylinderMesh";
	CylinderMesh.GenerateBoundingBox();
	MeshMap.emplace("CylinderMesh", std::move(CylinderMesh));

	MMesh GridMesh;
	GridMesh.CreateGrid(20.0f, 30.0f, 60, 40);
	GridMesh.MeshName = "GridMesh";
	GridMesh.GenerateBoundingBox();
	MeshMap.emplace("GridMesh", std::move(GridMesh));

	MMesh QuadMesh;
	QuadMesh.CreateQuad(-0.5f, 0.5f, 1.0f, 1.0f, 0.0f);
	QuadMesh.MeshName = "QuadMesh";
	QuadMesh.GenerateBoundingBox();
	MeshMap.emplace("QuadMesh", std::move(QuadMesh));

	MMesh ScreenQuadMesh;
	ScreenQuadMesh.CreateQuad(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);
	ScreenQuadMesh.MeshName = "ScreenQuadMesh";
	ScreenQuadMesh.GenerateBoundingBox();
	MeshMap.emplace("ScreenQuadMesh", std::move(ScreenQuadMesh));

	MMesh AssaultRifleMesh;
	AssaultRifleMesh.MeshName = "AssaultRifle";
	FbxLoader->LoadFBXMesh(L"LOW_WEPON.fbx", AssaultRifleMesh);
	AssaultRifleMesh.GenerateBoundingBox();
	MeshMap.emplace("AssaultRifle", std::move(AssaultRifleMesh));

	MMesh CyborgWeaponMesh;
	CyborgWeaponMesh.MeshName = "CyborgWeapon";
	FbxLoader->LoadFBXMesh(L"Cyborg_Weapon.fbx", CyborgWeaponMesh);
	CyborgWeaponMesh.GenerateBoundingBox();
	MeshMap.emplace("CyborgWeapon", std::move(CyborgWeaponMesh));

	MMesh HelmeMMesh;
	HelmeMMesh.MeshName = "Helmet";
	FbxLoader->LoadFBXMesh(L"helmet_low.fbx", HelmeMMesh);
	HelmeMMesh.GenerateBoundingBox();
	MeshMap.emplace("Helmet", std::move(HelmeMMesh));

	MMesh ColumnMesh;
	ColumnMesh.MeshName = "Column";
	FbxLoader->LoadFBXMesh(L"column.fbx", ColumnMesh);
	ColumnMesh.GenerateBoundingBox();
	MeshMap.emplace("Column", std::move(ColumnMesh));
}

void MMeshRepository::Unload()
{
	MeshMap.clear();
}