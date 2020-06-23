using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class Transform
    {
        public Transform()
        {
            m_Position = new Vector3();
            m_Rotation = new Vector3();
            m_Scale = new Vector3();
        }
        Vector3 m_Position;
        Vector3 m_Rotation;
        Vector3 m_Scale;

        public Vector3 Position { get { return m_Position; } set { m_Position = value; } }
        public Vector3 Rotation { get { return m_Rotation; } set { m_Rotation = value; } }
        public Vector3 Scale { get { return m_Scale; } set { m_Scale = value; } }

        public void SetActorPosition(float x, float y, float z)
        {
            m_Position.X = x;
            m_Position.Y = y;
            m_Position.Z = z;
        }

        public void TranslateActor(Vector3 offset)
        {
            m_Position.Plus(offset);
        }

        public void TranslateActor(float x, float y, float z)
        {
            m_Position.Plus(new Vector3(x, y, z));
        }

        public void RotateActorPitchYawRoll(Vector3 offset)
        {
            m_Rotation.Plus(offset);
        }

        public void RotateActorPitchYawRoll(float x, float y, float z)
        {
            m_Rotation.Plus(new Vector3(x, y, z));
        }

        public void ScaleActor(Vector3 offset)
        {
            m_Scale.Plus(offset);
        }

        public void ScaleActor(float x, float y, float z)
        {
            m_Scale.Plus(new Vector3(x, y, z));
        }

        public void SetActorScale(float x, float y, float z)
        {
            m_Scale.X = x;
            m_Scale.Y = y;
            m_Scale.Z = z;
        }

        public void SetActorRotation(float x, float y, float z)
        {
            m_Rotation.X = x;
            m_Rotation.Y = y;
            m_Rotation.Z = z;
        }
    }
}
