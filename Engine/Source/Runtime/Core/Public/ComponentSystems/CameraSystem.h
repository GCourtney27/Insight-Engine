#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ECS/EntityAdmin/SystemFwd.h"
#include "Runtime/Core/Public/ieObject/Components/ieCameraComponent.h"

namespace Insight
{
	class INSIGHT_API CameraSystem : public ECS::GenericSystem<ieCameraComponent>
	{
	public:
		CameraSystem(const ECS::EntityAdmin& EntityAdmin)
			: GenericSystem(EntityAdmin, "Camera System")
		{
		}
		~CameraSystem()
		{
		}

		virtual void Execute() override
		{
			UpdateCameras();
		}

	private:

		void UpdateCameras()
		{
			std::vector<ieCameraComponent>& Components = GetRawComponentData();

			for (ieCameraComponent& Camera : Components)
			{
				Camera.UpdateViewMat();
			}
		}

	};
}
