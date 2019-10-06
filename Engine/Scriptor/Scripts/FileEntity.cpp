#include "FileEntity.h"

FileEntity::FileEntity()
{

}

extern "C" FileEntity* factory(void)
{
	return new FileEntity;
}

//bool FileEntity::Start()
//{
//
//}
//
void FileEntity::Update(float deltaTime)
{
	//m_transform.AdjustRotation(0.0f, 1.0f * deltaTime, 0.0f);
}
//
//void FileEntity::Draw(const XMMATRIX & viewProjectionMatrix)
//{
//
//}
//
//void FileEntity::Destroy()
//{
//
//}