#pragma once
#include "pch.h"
#include "GameObj.h"

struct  SimpleVertex
{
    glm::vec3 pos = {};
    glm::vec3 color = {};
    SimpleVertex() = default;
    SimpleVertex(const glm::vec3& _v) :pos{ _v } {
        static std::mt19937 rng{std::random_device{}()};
        static std::uniform_int_distribution<> uid{0, 2};
        switch (uid(rng))
        {
        case 0:color = RGB_RED;
            break;
        case 1:color = RGB_GREEN;
            break;
        case 2:color = RGB_BLUE;
            break;
        }
    }
    operator glm::vec3() const { return pos; }
};

class Shader;

class MyPolygon :
    public GameObj
{
    friend class AssimpMgr;
    static shared_ptr<Shader> g_SimpleShader;
    static vector<shared_ptr<GameObj>> g_vecMainPoly;
private:
    vector<SimpleVertex> m_vecVertices;
    vector<GLuint> m_vecIdx;
    GLuint vao, vbo , ebo;
    GLint m_PolyType;
    glm::vec3 m_color = {};
protected:
    void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath)override;
    void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override;
    void PolygonSetBuffer();
public:
    MyPolygon(vector<SimpleVertex>&& verticies_, vector<GLuint>&& indices_);
    MyPolygon();
    MyPolygon(vector<SimpleVertex> _vertices);
    ~MyPolygon();
    void Render() override;
    void SetColor(const glm::vec3& _color) {
        for (auto& v : m_vecVertices)
        {
            v.color = _color;
        }
        m_color = _color;
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vecVertices.size() * sizeof(SimpleVertex), m_vecVertices.data(), GL_DYNAMIC_DRAW);
        glBindVertexArray(0);
    }
    void SetBuffer()
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vecVertices.size() * sizeof(SimpleVertex), m_vecVertices.data(), GL_DYNAMIC_DRAW);
        glBindVertexArray(0);
    }
    const glm::vec3& GetColor()const { return m_color; }
    std::span<SimpleVertex> GetVertices() { return m_vecVertices; }
    static void AddMainPoly(shared_ptr<MyPolygon> pPoly_) { g_vecMainPoly.emplace_back(std::move(pPoly_)); }
    static vector<MyPolygon*> GetMainPoly() 
    {
        vector<MyPolygon*> temp;
        for (auto& p : g_vecMainPoly)
        {
            temp.emplace_back(static_cast<MyPolygon*>(p.get()));
        }
        return temp; 
    }
};

