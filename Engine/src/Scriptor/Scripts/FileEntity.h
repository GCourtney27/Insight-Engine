#include "..\\..\\Objects\\Entity.h"

class FileEntity
{
public:
	FileEntity();
	~FileEntity() {}
	FileEntity* factory();

	/*bool Start() override;
	void Destroy() override;*/
	void Update(float deltaTime);

private:

};