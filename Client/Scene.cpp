#include "pch.h"
#include "Scene.h"
#include "GameObj.h"
#include "SceneMgr.h"
#include "Camera.h"
#include "CamMoveScript.h"
#include "Transform.h"
#include "Light.h"
#include "ThreadMgr.h"
#include "InputHandler.h"
#include "MoveCommand.h"
#include "PathMgr.h"
#include "ResMgr.h"
#include "Shader.h"
#include "Core.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Collider.h"
#include "SkyBox.h"

void Scene::EnterScene() noexcept
{
	m_fpEnterSceneCallBack();
	Awake();
	Start();
}

void Scene::ExitScene() noexcept
{
	for (auto& fp : m_arrAddedUpdateFp)
	{
		fp.clear();
	}
	for (auto& objs : m_vecObj)
	{
		objs.clear();
	}
	m_pCurPlayer = nullptr;
	for (auto& chunk : m_arrChunkMesh)
	{
		chunk.reset();
	}
	m_skyBox = nullptr;
	m_vecLights.clear();
}

Scene::Scene()
{
	auto pCam = GameObj::make_obj();
	pCam->GetTransform()->SetLocalPosition({ 0.f,1.f,-1.f });
	pCam->AddComponent<Camera>();
	pCam->AddScript(make_shared<CamMoveScript>());
	////static_pointer_cast<CamMoveScript>(pCam->GetMonoBehavior("CamMoveScript"))->SetMoveSpeed(0.f);
	////auto pHandle = pCam->AddComponent<InputHandler>();
	pCam->GetComp<Camera>()->SetMainCam();
	////pHandle->SetCommand(GLFW_KEY_W, make_shared<MoveFront>());
	////pHandle->SetCommand(GLFW_KEY_S, make_shared<MoveBack>());
	////pHandle->SetCommand(GLFW_KEY_A, make_shared<MoveLeft>());
	////pHandle->SetCommand(GLFW_KEY_D, make_shared<MoveRight>());
	////pHandle->SetCommand(GLFW_KEY_Q, make_shared<MoveLeftRotate>());
	////pHandle->SetCommand(GLFW_KEY_E, make_shared<MoveRightRotate>());
	////pHandle->SetCommand(GLFW_KEY_UP, make_shared<MoveUp>());
	////pHandle->SetCommand(GLFW_KEY_DOWN, make_shared<MoveDown>());
	AddObject(std::move(pCam), GROUP_TYPE::DEFAULT);
}

Scene::~Scene()
{
}

void Scene::AddObject(shared_ptr<GameObj> _pObj, GROUP_TYPE _eType)
{
	_pObj->InitGameObj();
	m_vecObj[etoi(_eType)].emplace_back(std::move(_pObj));
}

void Scene::Awake()
{
	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->Awake();
	}

	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			try {
				if (!obj->GetParentGameObj().expired())
					throw std::runtime_error("Dont Add Child in Scene");
			}
			catch (const std::runtime_error& e) {
				std::cerr << e.what();
				exit(0);
			}
		}
	}
	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			obj->Awake();
		}
	}
}

void Scene::Start()
{
	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->Start();
	}

	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			obj->Start();
		}
	}
}

void Scene::Update()
{
	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, std::mem_fn(&GameObj::Update));

	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->Update();
	}

	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			//Mgr(ThreadMgr)->Enqueue(&GameObj::Update, obj.get());
			if (obj->IsAlive())
			{
				obj->Update();
			}
			
		}
	
		//std::ranges::for_each(vec, std::mem_fn(&GameObj::Update));
	}

	//WaitUpdate();

	if (m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::UPDATE)])
	{
		m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::UPDATE)]();
	}
}

void Scene::LateUpdate()
{
	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, std::mem_fn(&GameObj::LateUpdate));

	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->LateUpdate();
	}


	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			if (obj->IsAlive())
			{
				obj->LateUpdate();
			}
		}
	}

	if (m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::LATEUPDATE)])
	{
		m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::LATEUPDATE)]();
	}
}

void Scene::LastUpdate()
{
	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, std::mem_fn(&GameObj::LastUpdate));

	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->LastUpdate();
	}


	for (auto& vec : m_vecObj)
	{
		for (auto& obj : vec)
		{
			if (obj->IsAlive())
			{
				obj->LastUpdate();
			}
		}
	}

	if (m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::LASTUPDATE)])
	{
		m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::LASTUPDATE)]();
	}

	//Mgr(ThreadMgr)->Join();
}

void Scene::PreFinalUpdate()
{
	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, [](const shared_ptr<GameObj>& obj)noexcept {
	//		Mgr(ThreadMgr)->Enqueue([obj = obj.get(), pTrans = obj->GetTransform().get()]()noexcept {
	//			obj->MarkTransformDirty();
	//			pTrans->UpdateTransfromHierarchy();
	//			});
	//	});

	for (const auto& vec : m_vecObj)
	{
		const auto cache = vec.data();
		const ushort num = (const ushort)vec.size();
		for (ushort i = 0; i < num; ++i)
		{
			if (cache[i]->IsAlive())
			{
				Mgr(ThreadMgr)->Enqueue([obj = cache[i].get(), pTrans = cache[i]->GetTransform().get()]()noexcept {
					obj->MarkTransformDirty();
					pTrans->UpdateTransfromHierarchy();
					});
			}
		}
	}

	Mgr(ThreadMgr)->WaitAllJob();

	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, [](const shared_ptr<GameObj>& obj)noexcept {
	//		Mgr(ThreadMgr)->Enqueue(&GameObj::ColliderUpdate, obj.get());
	//	});
	for (const auto& vec : m_vecObj)
	{
		const auto cache = vec.data();
		const ushort num = (const ushort)vec.size();
		for (ushort i = 0; i < num; ++i)
		{
			if (cache[i]->IsAlive())
			{
				Mgr(ThreadMgr)->Enqueue(&GameObj::ColliderUpdate, cache[i].get());
			}
		}
	}

	Mgr(ThreadMgr)->WaitAllJob();
}

void Scene::FinalUpdate()
{

	//std::ranges::for_each(m_vecObj
	//	| std::views::filter([](auto& p)noexcept{return nullptr != p ;})
	//	| ::OnlyAliveObject
	//	, [](const shared_ptr<GameObj>& obj)noexcept {
	//		Mgr(ThreadMgr)->Enqueue(&GameObj::FinalUpdate, obj.get());
	//	});

	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		Mgr(ThreadMgr)->Enqueue(&GameObj::FinalUpdate, chunk.get());
	}

	for (const auto& vec : m_vecObj)
	{
		const auto cache = vec.data();
		const ushort num = (const ushort)vec.size();
		for (ushort i = 0; i < num; ++i)
		{
			if (cache[i]->IsAlive())
			{
				Mgr(ThreadMgr)->Enqueue(&GameObj::FinalUpdate, cache[i].get());
			}
			//++m_iNumOfJob;
		}
	}

	//std::atomic_thread_fence(std::memory_order_seq_cst);
	//
	//const int jobCount = m_iNumOfJob;
	//
	//Mgr(ThreadMgr)->WaitAllJob(jobCount);
	//
	//std::atomic_thread_fence(std::memory_order_seq_cst);
	//
	//m_iNumOfJob = 0;
	Mgr(ThreadMgr)->WaitAllJob();
}

void Scene::PreRender()
{
	/*for (auto& vec : m_vecObj)
	{
		Mgr(ThreadMgr)->Enqueue([&vec]() {
			RemoveElementAll(vec, [](const shared_ptr<GameObj>& _pObj) {return !_pObj->IsAlive(); });
			});
	}*/

	//Mgr(ResMgr)->GetRes<Shader>("DefaultShader.glsl")->Use();

	//if (pCurShader_ == m_pPrevShader)
	//{
		//return;
	//}

	//m_pPrevShader = pCurShader_;

	static UBOData& sceneData = Mgr(Core)->GetUBOData();
	
	const auto pCam = Camera::GetCurCam();
	sceneData.projMat = pCam->GetCamMatProj();
	sceneData.viewMat = pCam->GetCamMatView();
	sceneData.viewPos = pCam->GetTransform()->GetWorldPosition();

	// convert game time into seconds
	auto gameTime = Mgr(TimeMgr)->GetGameTime();
	float tSeconds = gameTime.count() * 1e-9f;
	sceneData.time = tSeconds;

	if (const auto observer = Camera::GetObserverCam())
	{
		sceneData.observerPos = observer->GetTransform()->GetWorldPosition();
		sceneData.observerViewMat = observer->GetCamMatView();
	}
	else
	{
		sceneData.observerPos = sceneData.viewPos;
		sceneData.observerViewMat = sceneData.viewMat;
	}

	sceneData.lightCounts = glm::ivec4{ 0 };
	//static GLint curShaderID;
	//glGetIntegerv(GL_CURRENT_PROGRAM, &curShaderID);
	//GLuint lightCount = 0;
	const auto cache = m_vecLights.data();
	for (ushort idx = 0; idx < (const ushort)m_vecLights.size();)
	{
		if (const auto light = cache[idx].lock())
		{
			//light->PushLightData(curShaderID, lightCount++);
			/*Mgr(Core)->AddDrawCall([light = light.get(), lightCount]()noexcept {
				light->PushLightData(curShaderID, lightCount);
				});*/
			//sceneData.lights[idx].position = light->GetLightPos();
			//sceneData.lights[idx].ambient = light->GetLightAmbient();
			//sceneData.lights[idx].diffuse = light->GetLightDiffuse();
			//sceneData.lights[idx].specular = light->GetLightSpecular();
			//++lightCount;\

			if(!light->GetGameObj()->IsAlive())
			{
				idx = RemoveElement(m_vecLights, idx);
				continue;
			}
			light->GetCurLight()->PushLightData();
			++idx;
		}
		else
		{
			idx = RemoveElement(m_vecLights, idx);
		}
	}
	//sceneData.lightCount = lightCount;
	Mgr(Core)->BindUBOData();

	//glUniform1i(glGetUniformLocation(curShaderID, "lightCount"), lightCount);
	/*Mgr(Core)->AddDrawCall([lightCount]()noexcept {
		glUniform1i(glGetUniformLocation(curShaderID, "lightCount"), lightCount);
		});*/

	//Mgr(ThreadMgr)->Join();
	if (m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::PRERENDER)])
	{
		m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::PRERENDER)]();
	}
}

void Scene::Render()
{
	for (auto& chunk : m_arrChunkMesh | std::views::filter([](auto& p)noexcept{return nullptr != p ;}))
	{
		chunk->Render();
	}

	for (auto& vec : m_vecObj)
	{
		//for (const auto& obj : vec)
		//{
		//	//Mgr(ThreadMgr)->Enqueue(&GameObj::ClearComponentWaitFlag, obj);
		//	obj->Render();
		//}
		const auto cache = vec.data();
		for (ushort idx = 0; idx < (const ushort)vec.size();)
		{
			if (cache[idx]->IsAlive())
			{
				// Mgr(ThreadMgr)->Enqueue(&GameObj::ClearComponentWaitFlag, m_vecChildObj[idx].get());
				cache[idx]->Render();
				++idx;
			}
			else
			{
				idx = RemoveElement(vec, idx);
			}
		}
	}
	
	if (m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::RENDER)])
	{
		m_arrAddedUpdateFp[etoi(SCENE_ADDED_UPDATE::RENDER)]();
	}

	if (m_skyBox)
	{
		m_skyBox->Render();
	}
	//Mgr(ThreadMgr)->Join();
}

void Scene::SaveForPractice(string_view _strPracticeName)
{
	const auto practicePath = Mgr(PathMgr)->GetSavePath() / _strPracticeName;
	fs::create_directory(practicePath);
	
	//Mgr(ThreadMgr)->SetJobCount(0);

	int cnt = 0;

	int numOfObj = 0;

	for (int i = 1; i < etoi(GROUP_TYPE::END); ++i)
	{
		for (const auto& obj : m_vecObj[i])
		{
			++numOfObj;
		}
	}

	for (int i = 1 ; i< etoi(GROUP_TYPE::END);++i)
	{
		for (const auto& obj : m_vecObj[i])
		{
			obj->GameObj::Save(std::to_string(i) + "_" + std::to_string(cnt++), practicePath / "GameObj");
		}
	}

	//Mgr(ThreadMgr)->WaitAllJob(numOfObj);
	//Mgr(ThreadMgr)->Join();
}

void Scene::LoadForPractice(string_view _strPracticeName)
{
	const auto practicePath = Mgr(PathMgr)->GetSavePath() / _strPracticeName / "GameObj";

	fs::directory_iterator practiceIter{ practicePath };

	vector<std::future<pair<shared_ptr<GameObj>,ushort>>> temp;

	for (const auto& pracIter : practiceIter)
	{
		const string fileName = pracIter.path().filename().string();
		const ushort groupNum = std::stoi(fileName.substr(0, fileName.find('_')));
		temp.emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture([&practicePath,fileName,groupNum] {
			auto pObj = GameObj::make_obj();
			pObj->Load(fileName, practicePath);
			return std::make_pair(pObj, groupNum);
			}));
	}

	for (auto& f : temp)
	{
		auto [pObj, groupNum] = f.get();
		m_vecObj[groupNum].emplace_back(std::move(pObj));
	}
}

void Scene::SetSkyBox(SKYBOX_TYPE _eSkyBoxType,string_view _strSkyBoxName,string_view _strTextureName)
{
	m_skyBox = make_shared<SkyBox>(_eSkyBoxType,_strSkyBoxName, _strTextureName);
	m_skyBox->InitGameObj();
}

//void Scene::WaitUpdate() 
//{
//	std::ranges::for_each(m_vecObj, [](vector<shared_ptr<GameObj>>& vecObj) {
//		std::ranges::for_each(vecObj, [](shared_ptr<GameObj>& nodes) {
//			std::ranges::for_each(*nodes, std::mem_fn(&GameObj::WaitForObjUpdate));
//			});
//		});
//}
