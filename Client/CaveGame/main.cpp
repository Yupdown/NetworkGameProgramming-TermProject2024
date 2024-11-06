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

int main()
{
    Mgr(Core)->Init(1440, 720);
    Mgr(Core)->SetClearColor(RGBA_WHITE);

    Mgr(SceneMgr)->GetScene(SCENE_TYPE::INTRO)->AddUpdateFp(SCENE_ADDED_UPDATE::UPDATE,[]() {
        if (g_bCanResume && KEY_TAP(GLFW_KEY_ESCAPE))
        {
            glfwSetInputMode(Mgr(Core)->GetWinInfo(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ChangeScene(SCENE_TYPE::STAGE, false);
        }
        });

    Mgr(SceneMgr)->RegisterEnterSceneCallBack(SCENE_TYPE::STAGE, []() {
        g_bCanResume = true;

        shared_ptr<MCTilemap> tilemap = make_shared<MCTilemap>();
        shared_ptr<MCTerrainGenerator> terrainGenerator = make_shared<MCTerrainGenerator>();
        shared_ptr<MCTilemapMeshGenerator> meshGenerator = make_shared<MCTilemapMeshGenerator>();
        terrainGenerator->Generate(tilemap);

        glfwSetInputMode(Mgr(Core)->GetWinInfo(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        const auto curScene = Mgr(SceneMgr)->GetCurScene();

        shared_ptr<Material> chunkMaterial = make_shared<Material>();
        chunkMaterial->AddTexture2D("mctile.png");
        shared_ptr<Shader> chunkShader = Mgr(ResMgr)->GetRes<Shader>("DefaultWarpShader.glsl");

        // ûũ ������ ������Ʈ ���� �� �ʱ�ȭ
        for (auto& row : pChunkRenderers)
        {
            for (auto& pChunkRenderer : row)
            {
                pChunkRenderer = make_shared<ChunkRendererObject>();
                pChunkRenderer->Initialize(chunkShader, chunkMaterial);
                curScene->AddObject(pChunkRenderer, GROUP_TYPE::DEFAULT);
            }
        }
        // ûũ ������ ������Ʈ�� Ÿ�ϸ� ���濡 ���� �ݹ� �Լ� ���
        tilemap->AddNotifyCallback([tilemap, meshGenerator](MCTileChunk* pChunk, int chunkX, int chunkZ) {
            shared_ptr<Mesh> mesh = meshGenerator->CreateMeshFromChunk(tilemap, chunkX, chunkZ);
            pChunkRenderers[chunkX][chunkZ]->ChangeMeshData(mesh->GetVertices(), mesh->GetIndicies());
            });

        // Ÿ�ϸ� �޽� ���׷����ͷκ��� ��� ûũ�� ���� �޽� ����
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
            player->SetObjName("player");
            player->GetTransform()->SetLocalPosition(glm::vec3(256.0f, 16.0f, 256.0f));
            curScene->AddObject(player, GROUP_TYPE::PLAYER);
            /*player->AddChild(make_shared<PlayerCam>());*/
            curScene->RegisterPlayer(player);
            g_fpNewGameFunc = [player]() 
                {
                    glfwSetInputMode(Mgr(Core)->GetWinInfo(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    player->GetTransform()->SetLocalPosition(glm::vec3(256.0f, 16.0f, 256.0f));
                };//���⼭ ������ ������ �� � ������ �� ���ϱ� 
        } 

        pClouds = Mgr(AssimpMgr)->Load("CloudShader.glsl", "MyCube.fbx");
        pClouds->GetTransform()->SetLocalPosition(glm::vec3(256.0f, 100.0f, 256.0f));
        pClouds->GetTransform()->SetLocalScale(glm::vec3(320.0f, 1.0f, 320.0f));
        curScene->AddObject(pClouds, GROUP_TYPE::DEFAULT);

        shared_ptr<Material> material = make_shared<Material>();
        material->AddTexture2D("clouds.png");

        auto renderer = pClouds->GetComp<MeshRenderer>();
        renderer->AddMaterial(material);

        curScene->SetSkyBox(SKYBOX_TYPE::SPHERE, "basic_skybox_3d_flip.fbx", "skybox.png");

        auto pEnderEye = make_shared<EnderEye>();
        pEnderEye->GetTransform()->SetLocalPosition(glm::vec3(256.0f, 12.0f, 256.0f));
        pEnderEye->GetTransform()->SetLocalScale(glm::one<glm::vec3>() * 0.5f);
        curScene->AddObject(pEnderEye, GROUP_TYPE::DEFAULT);
        });

        Mgr(Core)->GameLoop([]()noexcept{
            // TODO: DoNetworkIO
            });
        
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
            glfwSetInputMode(Mgr(Core)->GetWinInfo(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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