#include "pch.h"
#include <MyOpenGL.hpp>

#include "MCTilemap.h"
#include "MCTerrainGenerator.h"
#include "MCTilemapMeshGenerator.h"
#include "ChunkRendererObject.h"

#include "Player.h"
#include "PlayerCam.h"

#include "Hero.h"
#include "EnderEye.h"
#include "NetworkMgr.h"
#include "PacketBase.hpp"
#include "ServerObjectManager.h"
#include "ProjectileArrow.h"
#include "ServerObjectFactory.h"
#include "MCItemManager.h"
#include "MCItemTable.h"
#include "EnderDragonRenderer.h"

shared_ptr<GameObj> pObserver;
shared_ptr<GameObj> pClouds;
shared_ptr<Camera>  observerCam;

array<array<shared_ptr<ChunkRendererObject>, MCTilemap::CHUNK_SIZE>, MCTilemap::CHUNK_SIZE> pChunkRenderers;

bool g_bWireFrame = false;
bool g_bCanResume = false;
bool g_bCamMode = false;

std::atomic_bool g_bTileFinish = false;

void Update();
std::function<void(void)> g_fpNewGameFunc;

int G_MC_SEED = -1;

glm::vec3 G_INIT_POS = glm::vec3(MCTilemap::MAP_WIDTH / 2, MCTilemap::MAP_HEIGHT, MCTilemap::MAP_WIDTH / 2);

int main()
{
    Mgr(Core)->Init(1280, 720);
    Mgr(Core)->SetClearColor(RGBA_WHITE);
    MCItemTable::Init();
    Mgr(MCItemManager)->LoadItems();

    if (false == Mgr(NetworkMgr)->InitClient("127.0.0.1", "8888"))
    {
        std::cout << "서버와 연결 실패\n";
        return 1;
    }
    
   Send(c2s_LOGIN{});

    // SEED 받기
    while(-1 == G_MC_SEED){ Mgr(NetworkMgr)->IORoutine(); }

    Mgr(ServerObjectManager)->SetTileMap(make_shared<MCTilemap>());
    ServerObjectFactory::SetTileMap(GetTileMapGlobal());

    Mgr(SceneMgr)->GetScene(SCENE_TYPE::INTRO)->AddUpdateFp(SCENE_ADDED_UPDATE::UPDATE,[]() {
        if (g_bCanResume && KEY_TAP(GLFW_KEY_ESCAPE))
        {
            ChangeScene(SCENE_TYPE::STAGE, false);
        }
        });

    Mgr(SceneMgr)->RegisterEnterSceneCallBack(SCENE_TYPE::STAGE, []() {
        g_bCanResume = true;

        shared_ptr<MCTilemap> tilemap = Mgr(ServerObjectManager)->GetTileMap();
        shared_ptr<MCTerrainGenerator> terrainGenerator = make_shared<MCTerrainGenerator>();
        shared_ptr<MCTilemapMeshGenerator> meshGenerator = make_shared<MCTilemapMeshGenerator>();
        terrainGenerator->Generate(tilemap);

        const auto curScene = Mgr(SceneMgr)->GetCurScene();

        shared_ptr<Material> chunkMaterial = make_shared<Material>();
        chunkMaterial->AddTexture2D("mctile.png");
        shared_ptr<Shader> chunkShader = Mgr(ResMgr)->GetRes<Shader>("DefaultWarpShader.glsl");

        // 청크 렌더러 오브젝트 생성 및 초기화
        for (auto& row : pChunkRenderers)
        {
            for (auto& pChunkRenderer : row)
            {
                pChunkRenderer = make_shared<ChunkRendererObject>();
                pChunkRenderer->Initialize(chunkShader, chunkMaterial);
                curScene->AddObject(pChunkRenderer, GROUP_TYPE::DEFAULT);
            }
        }
        // 청크 렌더러 오브젝트에 타일맵 변경에 대한 콜백 함수 등록
        tilemap->AddNotifyCallback([tilemap, meshGenerator](MCTileChunk* pChunk, int chunkX, int chunkZ) {
            shared_ptr<Mesh> mesh = meshGenerator->CreateMeshFromChunk(tilemap, chunkX, chunkZ);
            pChunkRenderers[chunkX][chunkZ]->ChangeMeshData(mesh->GetVertices(), mesh->GetIndicies());
            });

        // 타일맵 메쉬 제네레이터로부터 모든 청크에 대한 메쉬 생성
        meshGenerator->CreateMeshAll(tilemap, pChunkRenderers);

        curScene->AddUpdateFp(SCENE_ADDED_UPDATE::UPDATE, &Update);
        {
            auto pLight = Mgr(AssimpMgr)->Load("EnvironmentShader.glsl", "MyCube.fbx");
            auto l = pLight->AddComponent<Light>();
            l->SetCurLightType(LIGHT_TYPE::DIRECTIONAL);
            l->SetLightPos({ -5,5,0 });
            auto mate = make_shared<Material>();
            mate->AddTexture2D("skybox.png");
            mate->SetMaterialDiffuse({ .3f,.3f,.3f });
            mate->SetMaterialSpecular({ .3f,.3f,.3f });
            pLight->GetComp<MeshRenderer>()->AddMaterial(mate);
            pLight->GetTransform()->SetLocalScale(0.1f);
            pLight->GetTransform()->SetLocalRotation(45.0f, X_AXIS);
            pLight->GetTransform()->AddLocalRotation(1.0f, Y_AXIS);
            l->SetAmbient(glm::vec3{ 0.5f, 0.5f, 0.5f });
            l->SetDiffuse(glm::vec3{ 1.0f, 1.0f, 1.0f });
            curScene->AddObject(pLight, GROUP_TYPE::MONSTER);
            pLight->SetObjName("light");
            auto pCol = pLight->AddComponent<Collider>();
            pCol->SetColBoxScale({ 5,5,5 });

            Mgr(RayCaster)->RegisterMeshRayIntersect(pLight);
            // Mgr(InstancingMgr)->AddInstancingList(pLight);

            pCol->GetCollisionHandler()->SetCollisionHandlerFunc([l, pLight](auto& a, auto& b) {
                static auto s = l->GetLightSpecular();
                static auto d = l->GetLightDiffuse();
                static bool flag = false;
                flag = !flag;
                if (flag)
                {
                    l->SetSpecular({});
                    l->SetDiffuse({});
                }
                else
                {
                    l->SetSpecular(s);
                    l->SetDiffuse(d);
                }
                }, COLLISION_TYPE::COL_ENTER);

        }
        {
            auto pLight = Mgr(AssimpMgr)->Load("EnvironmentShader.glsl", "MySphereRed.fbx");
            //auto l = pLight->AddComponent<Light>();
            //l->SetCurLightType(LIGHT_TYPE::POINT);
            //l->SetDiffuse({ 5.1f,.1f,.1f });
            //l->SetSpecular({ 5.1f,.1f,.1f });
            auto m = pLight->GetComp<MeshRenderer>();
            auto mate = make_shared<Material>();
            mate->AddTexture2D("skybox.png");
            mate->SetMaterialDiffuse({ .3f,.3f,.3f });
            mate->SetMaterialSpecular({ .3f,.3f,.3f });
            m->AddMaterial(mate);
            pLight->GetTransform()->SetLocalScale(0.1f);
            pLight->GetTransform()->SetLocalRotation(90.0f, X_AXIS);
            pLight->GetTransform()->SetLocalPosition({ 0,5,0 });
            curScene->AddObject(pLight, GROUP_TYPE::MONSTER);
        }

        Mgr(CollisionMgr)->RegisterGroup(GROUP_TYPE::MONSTER, GROUP_TYPE::PROJ_PLAYER);
        {
            auto player = make_shared<Hero>(tilemap);
          
            Mgr(ServerObjectManager)->SetHero(player);

            player->SetObjName("player");
            player->SetRendererTexture(Mgr(ServerObjectManager)->GetMyTexIDFromServer());

            // 플레이어 스폰 위치를 맵의 정중앙, 하늘 위로 설정
            player->GetTransform()->SetLocalPosition(G_INIT_POS);
            curScene->AddObject(player, GROUP_TYPE::PLAYER);
            /*player->AddChild(make_shared<PlayerCam>());*/
            curScene->RegisterPlayer(player);
            g_fpNewGameFunc = [player]() 
                {
                    Mgr(KeyMgr)->SetMouseMode(GLFW_CURSOR_DISABLED);
                    player->GetTransform()->SetLocalPosition(glm::vec3(256.0f, 16.0f, 256.0f));
                };//여기서 뉴게임 눌렀을 때 어떤 반응할 지 정하기 
        } 

        pClouds = Mgr(AssimpMgr)->Load("CloudShader.glsl", "MyCube.fbx");
        pClouds->GetTransform()->SetLocalPosition(glm::vec3(MCTilemap::MAP_WIDTH / 2, 100.0f, MCTilemap::MAP_WIDTH / 2));
        pClouds->GetTransform()->SetLocalScale(glm::vec3(320.0f, 1.0f, 320.0f));
        curScene->AddObject(pClouds, GROUP_TYPE::DEFAULT);

        shared_ptr<Material> material = make_shared<Material>();
        material->AddTexture2D("clouds.png");

        auto renderer = pClouds->GetComp<MeshRenderer>();
        renderer->AddMaterial(material);

        curScene->SetSkyBox(SKYBOX_TYPE::SPHERE, "basic_skybox_3d_flip.fbx", "skybox.png");

        auto pEnderEye = make_shared<EnderEye>();
        pEnderEye->GetTransform()->SetLocalPosition(glm::vec3(MCTilemap::MAP_WIDTH / 2, 16.0f, MCTilemap::MAP_WIDTH / 2));
        pEnderEye->GetTransform()->SetLocalScale(glm::one<glm::vec3>() * 0.5f);
        curScene->AddObject(pEnderEye, GROUP_TYPE::DEFAULT);

        Mgr(ServerObjectManager)->SetTargetScene(Mgr(SceneMgr)->GetScene(SCENE_TYPE::STAGE));
        Send(c2s_ENTER{});
        });

        Mgr(Core)->GameLoop([]()noexcept {Mgr(NetworkMgr)->IORoutine(); });
        
}

void Update()
{
    if (KEY_TAP(GLFW_KEY_F3))
    {
        g_bWireFrame = !g_bWireFrame;
        glPolygonMode(GL_FRONT_AND_BACK, g_bWireFrame ? GL_LINE : GL_FILL);
    }
    if (KEY_TAP(GLFW_KEY_ESCAPE))
    {
        if (g_bTileFinish)
        {
            Mgr(KeyMgr)->SetMouseMode(GLFW_CURSOR_NORMAL);
            UnLoadScene();
        }
    }
    if (KEY_TAP(GLFW_KEY_T))
    {
        g_bCamMode = !g_bCamMode;
        Camera::GetCurCam()->StartChangeCamProjType();
        const auto curScene = Mgr(SceneMgr)->GetCurScene();
        if (g_bCamMode)
            DestroyObj(pClouds);
        else
            curScene->AddObject(pClouds, GROUP_TYPE::DEFAULT);
    }
}