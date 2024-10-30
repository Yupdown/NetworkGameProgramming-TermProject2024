#include "pch.h"
#include "MyPolygon.h"
#include "ResMgr.h"
#include "Shader.h"
#include "Transform.h"
#include "PathMgr.h"
#include "Core.h"
#include "Camera.h"

shared_ptr<Shader> MyPolygon::g_SimpleShader;
vector<shared_ptr<GameObj>> MyPolygon::g_vecMainPoly;

MyPolygon::MyPolygon()
{
    if (!g_SimpleShader)
    {
        MyPolygon::g_SimpleShader = Mgr(ResMgr)->GetRes<Shader>("SimpleShader.glsl");
    }

    g_SimpleShader->Use();
}

MyPolygon::MyPolygon(vector<SimpleVertex> _vertices)
    :m_vecVertices{ std::move(_vertices) }
{
    if (!g_SimpleShader)
    {
        MyPolygon::g_SimpleShader = Mgr(ResMgr)->GetRes<Shader>("SimpleShader.glsl");
    }

    g_SimpleShader->Use();
   
    const size_t ShapeType = m_vecVertices.size();

    m_vecIdx.emplace_back(0);
    m_vecIdx.emplace_back(1);

    switch (ShapeType)
    {
    case 2:
    {
        m_PolyType = GL_LINES;
    }
        break;
    case 3:
    {
        m_PolyType = GL_TRIANGLES;
        m_vecIdx.emplace_back(2);
    }
    break;
    case 4:
    {
        m_PolyType = GL_TRIANGLES;
        m_vecIdx.emplace_back(2);
        m_vecIdx.emplace_back(0);
        m_vecIdx.emplace_back(2);
        m_vecIdx.emplace_back(3);
    }
    break;
    }

    PolygonSetBuffer();
}

MyPolygon::~MyPolygon()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void MyPolygon::Render()
{
    g_SimpleShader->Use();
    GameObj::Render();
    glBindVertexArray(vao);
    g_SimpleShader->SetUniformMat4(GetTransform()->GetLocalToWorldMatrix(), "uModel");
    glDrawElements(m_PolyType, (GLsizei)m_vecIdx.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    /*if (m_vecVertices.empty() || m_vecIdx.empty())
    {
        return;
    }
    Mgr(Core)->AddDrawCall([this]()noexcept {
        g_SimpleShader->Use();
        glBindVertexArray(vao);
        g_SimpleShader->SetUniformMat4(GetTransform()->GetLocalToWorldMatrix(), "uModel");
        glDrawElements(m_PolyType, (GLsizei)m_vecIdx.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        });*/
}

void MyPolygon::PolygonSetBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vecVertices.size() * sizeof(SimpleVertex), m_vecVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, color));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIdx.size() * sizeof(GLuint), m_vecIdx.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

MyPolygon::MyPolygon(vector<SimpleVertex>&& verticies_, vector<GLuint>&& indices_)
    :m_vecVertices{std::move(verticies_)},m_vecIdx{std::move(indices_)},m_PolyType{ GL_TRIANGLES }
{
    if (!g_SimpleShader)
    {
        MyPolygon::g_SimpleShader = Mgr(ResMgr)->GetRes<Shader>("SimpleShader.glsl");
    }

    g_SimpleShader->Use();

    if (m_vecVertices.empty() || m_vecIdx.empty())
    {
        return;
    }

    PolygonSetBuffer();
}

void MyPolygon::Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath)
{
    std::ofstream outFile((_savePath / _resName / _resName).string() + "_Polygon.json");
    
    nlohmann::json polygonJson;

    polygonJson["Vertices"] = nlohmann::json::array();

    for (auto& v : m_vecVertices)
    {
        polygonJson["Vertices"].push_back({ v.pos.x, v.pos.y, v.pos.z });
    }

    polygonJson["Indices"] = nlohmann::json::array();

    for (auto& i : m_vecIdx)
    {
        polygonJson["Indices"].push_back(i);
    }

    outFile << polygonJson.dump(4);

    GameObj::Save(_resName, writer,_savePath);
}

void MyPolygon::Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath)
{
    std::ifstream inFile((_loadPath / _dirName / _dirName).string() + "_Polygon.json");

    nlohmann::json polygonJson;

    inFile >> polygonJson;

    for (const auto& v : polygonJson["Vertices"])
    {
        SimpleVertex vertex; 
        vertex.pos.x = v[0];
        vertex.pos.y = v[1];
        vertex.pos.z = v[2];
        m_vecVertices.emplace_back(vertex);
    }

    for (const auto& i : polygonJson["Indices"])
    {
        m_vecIdx.emplace_back(i);
    }

    const size_t ShapeType = m_vecVertices.size();

    switch (ShapeType)
    {
    case 2:
    {
        m_PolyType = GL_LINE;
    }
    break;
    case 3:
    {
        m_PolyType = GL_TRIANGLES;
    }
    break;
    case 4:
    {
        m_PolyType = GL_TRIANGLES;
    }
    break;
    }

    PolygonSetBuffer();

    GameObj::Load(_dirName, doc,_loadPath);
}
