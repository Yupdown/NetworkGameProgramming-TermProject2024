#pragma once

class GameObj;
class Light;
class Collider;

enum class SCENE_ADDED_UPDATE
{
	UPDATE,
	LATEUPDATE,
	LASTUPDATE,

	PRERENDER,
	RENDER,

	END
};

class SkyBox;

class Scene
{
private:
	array<shared_ptr<GameObj>, 36> m_arrChunkMesh;
	
	vector<shared_ptr<GameObj>> m_vecObj[etoi(GROUP_TYPE::END)];
	vector<weak_ptr<Light>> m_vecLights;
	shared_ptr<SkyBox> m_skyBox;
	array<Delegate, etoi(SCENE_ADDED_UPDATE::END)> m_arrAddedUpdateFp;
	shared_ptr<GameObj> m_pCurPlayer;
	Delegate m_fpEnterSceneCallBack;
public:

	virtual void EnterScene()noexcept;
	virtual void ExitScene()noexcept;

	vector<shared_ptr<GameObj>>& GetGroupObj(GROUP_TYPE _eType) { return m_vecObj[etoi(_eType)]; }
	Scene();
	virtual ~Scene();
	void AddObject(shared_ptr<GameObj> _pObj, GROUP_TYPE _eType);

	void Awake();
	void Start();

	void Update();
	void LateUpdate();
	void LastUpdate();

	void PreFinalUpdate();

	void FinalUpdate();

	void PreRender();
	void Render();

	void AddLights(weak_ptr<Light> _pLight) { m_vecLights.emplace_back(std::move(_pLight)); }

	template<typename Fp>
	void AddUpdateFp(SCENE_ADDED_UPDATE _eUpdateType, Fp&& fp)noexcept
	{
		m_arrAddedUpdateFp[etoi(_eUpdateType)] += [fp = std::move(fp)]()mutable { fp(); };
	}

	template<typename Func, typename... Args> requires std::invocable<Func, Args...>
	void AddUpdateFp(SCENE_ADDED_UPDATE _eUpdateType,Func&& fp, Args&&... args)
	{
		m_arrAddedUpdateFp[etoi(_eUpdateType)] += [fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable {std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); };
	}

	void SaveForPractice(string_view _strPracticeName);
	void LoadForPractice(string_view _strPracticeName);
	//void WaitUpdate();
	void SetSkyBox(SKYBOX_TYPE _eSkyBoxType,string_view _strSkyBoxName, string_view _strTextureName);

	void AddChunkMesh(const uint iTexID, shared_ptr<GameObj> pChunkMesh)noexcept
	{
		m_arrChunkMesh[iTexID] = std::move(pChunkMesh);
	}
	void RegisterPlayer(shared_ptr<GameObj> pPlayer)noexcept { m_pCurPlayer = pPlayer; }
	const shared_ptr<GameObj>& GetPlayer()const noexcept { return m_pCurPlayer; }

	template<typename Func, typename... Args> requires std::invocable<Func, Args...>
	void RegisterEnterSceneCallBack(Func&& fp, Args&&... args)noexcept
	{
		m_fpEnterSceneCallBack += 
			[fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable noexcept
			{std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); };
	}
};

