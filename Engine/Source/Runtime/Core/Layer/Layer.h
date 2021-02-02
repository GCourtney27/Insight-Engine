#pragma once

#include <Runtime/CoreMacros.h>
#include "Runtime/Events/Event.h"
#include "Platform/DirectX12/RenderPasses/RenderPasses.h"

namespace Insight {

	class INSIGHT_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		/*
			Called when the layer is attached to the layer stack.
		*/
		virtual void OnAttach() {}
		
		/*
			Called when the layer is detached to the layer stack.
		*/
		virtual void OnDetach() {}

		/*
			Called once per frame when the layer stack is updated.
		*/
		virtual void OnUpdate(const float DeltaTime) {}

		/*
			Called once per frame when the world begins its render pass.
		*/
		virtual void OnRender() {}

		/*
			Process events.
		*/
		virtual void OnEvent(Event& Event) {}

		/*
			Renders any panels the layer might need for the editor.
		*/
		virtual void OnImGuiRender() {}

		/*
			Returns the debug name for this layer.
		*/
		inline const std::string& GetName() const { return m_DebugName; }


	protected:

		/*
			The debug name for the layer.
		*/
		std::string m_DebugName;
	};

}