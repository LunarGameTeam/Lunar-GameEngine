#pragma once
#include "gltf_scene_loader.h"
#include "gltf_mesh_loader.h"
#include "gltf_skeleton_loader.h"
#include "gltf_animation_loader.h"
#include "gltf_material_loader.h"
#include "gltf_camera_loader.h"
namespace luna::lgltf
{
    class StreamReaderWriter : public Microsoft::glTF::IStreamReader
    {
        LString mResPath;
    public:
        StreamReaderWriter(LString resPath)
            : m_streams()
        {
            mResPath = resPath;
        }

        std::shared_ptr<std::istream> GetInputStream(const std::string& uri) const override
        {
            return GetStream(mResPath.c_str() + std::string("/") + uri);
        }
    private:
        std::shared_ptr<std::ifstream> GetStream(const std::string& uri) const
        {
            if (m_streams.find(uri) == m_streams.end())
            {
                m_streams[uri] = std::make_shared<std::ifstream>(uri,std::ios::binary);
            }
            return m_streams[uri];
        }

        mutable std::unordered_map<std::string, std::shared_ptr<std::ifstream>> m_streams;
    };

    inline std::shared_ptr<std::stringstream> ReadLocalAsset(const std::string& relativePath)
    {
        std::string filename = relativePath.c_str();

        // Read local file
        int64_t m_readPosition = 0;
        std::shared_ptr<const std::vector<int8_t>> m_buffer;
        std::ifstream ifs;
        ifs.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);
        if (ifs.is_open())
        {
            std::streampos start = ifs.tellg();
            ifs.seekg(0, std::ios::end);
            m_buffer = std::make_shared<const std::vector<int8_t>>(static_cast<unsigned int>(ifs.tellg() - start));
            ifs.seekg(0, std::ios::beg);
            ifs.read(reinterpret_cast<char*>(const_cast<int8_t*>(m_buffer->data())), m_buffer->size());
            ifs.close();
        }
        else
        {
            throw std::runtime_error("Could not open the file for reading");
        }

        // To IStream
        unsigned long writeBufferLength = 4096L * 1024L;
        auto tempStream = std::make_shared<std::stringstream>();
        auto tempBuffer = new char[writeBufferLength];
        // Read the file for as long as we can fill the buffer completely.
        // This means there is more content to be read.
        unsigned long bytesRead;
        do
        {
            auto bytesAvailable = m_buffer->size() - m_readPosition;
            unsigned long br = std::min(static_cast<unsigned long>(bytesAvailable), writeBufferLength);
#ifdef _WIN32
            memcpy_s(tempBuffer, br, m_buffer->data() + m_readPosition, br);
#else
            memcpy(tempBuffer, m_buffer->data() + m_readPosition, br);
#endif
            m_readPosition += br;
            bytesRead = br;

            tempStream->write(tempBuffer, bytesRead);
        } while (bytesRead == writeBufferLength);

        delete[] tempBuffer;

        if (tempStream.get()->bad())
        {
            throw std::runtime_error("Bad std::stringstream after copying the file");
        }

        return tempStream;
    }

	LGltfLoaderFactory::LGltfLoaderFactory()
	{
		mLoaders.insert(std::pair<LGltfDataType, std::shared_ptr<LGltfLoaderBase>>(LGltfDataType::GltfMeshData, std::make_shared<LGltfLoaderMesh>()));
	}

	std::shared_ptr<LGltfDataBase> LGltfLoaderFactory::LoadGltfData(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, LGltfDataType type, size_t gltfDataId)
	{
		auto needLoader = mLoaders.find(type);
		if (needLoader == mLoaders.end())
		{
			return nullptr;
		}
		return needLoader->second->ParsingData(doc, reader, gltfDataId);
	}

	void LGltfLoaderHelper::LoadGltfFile(const char* pFilename, LGltfSceneData& sceneOut)
	{
        LString pathName = pFilename;
        size_t rootPathIndex = std::string::npos;
        for (int32_t i = 0; i < pathName.Length(); ++i)
        {
            if (pathName[i] == '\\')
            {
                pathName[i] = '/';
            }
            if(pathName[i] == '/')
            {
                rootPathIndex = i;
            }
        }
        LString resPath = "";
        if(rootPathIndex != std::string::npos)
        {
            resPath = pathName.Substr(0, rootPathIndex);
        }
        
        auto input = ReadLocalAsset(pFilename);
      
        std::shared_ptr<StreamReaderWriter> readwriter = std::make_shared<StreamReaderWriter>(resPath);
        Microsoft::glTF::GLTFResourceReader resourceReader(readwriter);
        std::string json = std::string(std::istreambuf_iterator<char>(*input), std::istreambuf_iterator<char>());
		Microsoft::glTF::Document doc = Microsoft::glTF::Deserialize(json);
        //处理父节点
        std::unordered_map<size_t, size_t> parentIndex;
        for (size_t nodeIndex = 0; nodeIndex < doc.nodes.Size(); ++nodeIndex)
        {
            size_t nodeIndexNum = std::atoi(doc.nodes[nodeIndex].id.c_str());
            assert(nodeIndexNum == nodeIndex);
            for (size_t childIndex = 0; childIndex < doc.nodes[nodeIndex].children.size(); ++childIndex)
            {
                size_t realIndex = std::atoi(doc.nodes[nodeIndex].children[childIndex].c_str());
                parentIndex[realIndex] = nodeIndex;
            }
        }
        //添加节点信息
        std::unordered_map<LGltfDataType,std::unordered_map<size_t, size_t>> gltfDataToCommonData;
        for(size_t nodeIndex = 0; nodeIndex < doc.nodes.Size(); ++nodeIndex)
        {
            LGltfNodeBase newNode;
            newNode.mIndex = nodeIndex;
            newNode.matrix = doc.nodes[nodeIndex].matrix;
            newNode.translation = doc.nodes[nodeIndex].translation;
            newNode.rotation = doc.nodes[nodeIndex].rotation;
            newNode.scale = doc.nodes[nodeIndex].scale;
            for (size_t childIndex = 0; childIndex < doc.nodes[nodeIndex].children.size(); ++childIndex)
            {
                size_t realIndex = std::atoi(doc.nodes[nodeIndex].children[childIndex].c_str());
                newNode.mChild.push_back(realIndex);
            }
            newNode.mName = doc.nodes[nodeIndex].name;
            newNode.mParent = parentIndex[nodeIndex];
            if (doc.nodes[nodeIndex].meshId != "")
            {
                //添加mesh信息
                if (gltfDataToCommonData.find(LGltfDataType::GltfMeshData) == gltfDataToCommonData.end())
                {
                    gltfDataToCommonData.emplace(LGltfDataType::GltfMeshData, std::unordered_map<size_t, size_t>());
                }
                std::unordered_map<size_t, size_t>& gltfMeshDataToCommonData = gltfDataToCommonData[LGltfDataType::GltfMeshData];
                size_t gltfMeshIndex = std::atoi(doc.nodes[nodeIndex].meshId.c_str());
                size_t dataResIndex = sceneOut.mDatas.size();
                auto gltfMeshExist = gltfMeshDataToCommonData.find(gltfMeshIndex);
                if (gltfMeshExist == gltfMeshDataToCommonData.end())
                {
                    gltfMeshDataToCommonData[gltfMeshIndex] = dataResIndex;
                    std::shared_ptr<LGltfDataBase> meshData = singleLoaderInterface->LoadGltfData(doc, resourceReader,LGltfDataType::GltfMeshData, gltfMeshIndex);
                    meshData->SetNodeIndex(nodeIndex);
                    sceneOut.mDatas.push_back(meshData);
                }
                else
                {
                    dataResIndex = gltfMeshExist->second;
                }
                newNode.mNodeData.insert(std::pair<LGltfDataType, size_t>(LGltfDataType::GltfMeshData, dataResIndex));
            }
            sceneOut.mNodes.push_back(newNode);
        }
        int a = 0;
	}
}