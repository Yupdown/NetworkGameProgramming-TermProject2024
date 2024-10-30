#pragma once

struct alignas(16) LightData
{
	glm::vec3 position;
	float pad1; 
	glm::vec3 ambient;
	float pad2;
	glm::vec3 diffuse;
	float pad3;
	glm::vec3 specular;
	float pad4;
	glm::vec3 direction;
	float pad5;
};

struct alignas(16) PointLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
	float pad;	
};

struct alignas(16) SpotLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutoff;
	float pad[3];
};

struct alignas(16) DirectionalLightData
{
	LightData lData;
};

struct alignas(16) UBOData
{
	glm::mat4 projMat;
	glm::mat4 viewMat;
	glm::vec3 viewPos;
	float pad1;
	PointLightData pointLights[20];
	SpotLightData spotLights[20];
	DirectionalLightData dirLight;
	glm::mat4 observerViewMat = glm::mat4{ 1.f };
	glm::vec3 observerPos = glm::vec3{ 0.f,0.f,0.f };
	float pad2;
	glm::ivec4 lightCounts = glm::ivec4{ 0 };
};

void LowFragmentationHeap()noexcept;

class Core
	:public Singleton<Core>
{
	friend class Singleton;
	Core();
	~Core();
	void SetWinWidth(const int width) { m_winWidth = width; }
	void SetWinHeight(const int height) { m_winHeight = height; }
private:
	GLFWwindow* m_pWinInfo = nullptr;
	GLuint m_winWidth = {};
	GLuint m_winHeight = {};
	glm::vec4 m_ClearColor = RGBA_GRAY;
	//vector<std::future<void>> m_vecDrawCall;
	UBOData m_uboMatLight;
	GLuint m_ubo;
	HWND m_hWnd;
	HDC m_hDC;       
	HGLRC m_hglrc;
	mutable float m_fScaleFactor = 1.f;
	mutable float m_fScaleFactorW = 1.f;
	mutable float m_fScaleFactorH = 1.f;
private:
	void SetUBO();
	std::tuple<int, int, int, int> AdjustWinSize(GLuint width_, GLuint height_, HWND hwnd)const noexcept;
	std::tuple<int, int, int, int> AdjustWinSize(HWND hwnd)const noexcept;
	void PrepareStart()const noexcept;
public:
	const auto GetScaleFactorWH()const noexcept { return std::make_pair(m_fScaleFactorW, m_fScaleFactorH); }
	const float GetScaleFactor()const noexcept { return m_fScaleFactor; }
	void SetScaleFactorW(const float _fX)noexcept { m_fScaleFactorW = _fX; }
	void SetScaleFactorH(const float _fY)noexcept { m_fScaleFactorH = _fY; }

	void BindUBOData()const noexcept {
		glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOData), &m_uboMatLight);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	UBOData& GetUBOData()noexcept { return m_uboMatLight; }
	const GLuint GetUBO_ID()const noexcept { return m_ubo; }
	void Init(const GLuint _winWidth = 0, const GLuint _winHeight = 0);
	void GameLoop();

	GLFWwindow* GetWinInfo()const { return m_pWinInfo; }
	GLuint GetWidth()const { return m_winWidth; }
	GLuint GetHeight()const { return m_winHeight; }
	
	void SetClearColor(const glm::vec4& _color) {
		m_ClearColor = _color;
		glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
	}
	const glm::vec4& GetClearColor()const { return m_ClearColor; }

	const std::pair<float, float> GetWidthHeight()const { return make_pair((float)m_winWidth, (float)m_winHeight); }

	void Quit()const noexcept;

	bool IsBound(const glm::vec2& _lt, const glm::vec2& _rb, glm::vec2* _vResVec = nullptr)
	{
		const bool ltCollision = _lt.x <= 0 || _lt.y <= 0;
		const bool rbCollision = _rb.x >= (float)m_winWidth || _rb.y >= (float)m_winHeight;
		if (_vResVec)
		{
			if (_lt.x <= 0)(*_vResVec).x = 1.f;
			if (_lt.y <= 0)(*_vResVec).y = 1.f;
			if (_rb.x >= (float)m_winWidth)(*_vResVec).x = -1.f;
			if (_rb.y >= (float)m_winHeight)(*_vResVec).y = -1.f;
		}
		return ltCollision || rbCollision;
	}

	bool IsBound(const glm::vec2& _rectCenter, const float _width, const float _height,glm::vec2* _vResVec = nullptr)
	{
		const bool bRes = IsBound(_rectCenter - glm::vec2{ _width,_height } / 2.f, _rectCenter + glm::vec2{ _width,_height } / 2.f,_vResVec);
		if (_vResVec && bRes)
		{
			(*_vResVec).x == 1.f ? (*_vResVec).x = ((_width/2.f) + 1.f) : (*_vResVec).x *= -((float)m_winWidth - (_width/2.f) - 1.f);
			(*_vResVec).y == 1.f ? (*_vResVec).y = ((_height/2.f) + 1.f) : (*_vResVec).y *= -((float)m_winHeight - (_height/2.f) - 1.f);
		}
		return bRes;
	}

	/*template <typename DrawFunc>
	inline void AddDrawCall(DrawFunc&& drawCall_)noexcept { m_vecDrawCall.emplace_back(std::async(std::launch::deferred,std::move(drawCall_))); }
	inline void DrawCall()noexcept
	{
		const auto cache = m_vecDrawCall.data();
		const unsigned int num = (unsigned int)m_vecDrawCall.size();
		for (unsigned int i = 0; i < num; ++i)
		{
			cache[i].get();
		}
		m_vecDrawCall.clear();
	}*/
};

