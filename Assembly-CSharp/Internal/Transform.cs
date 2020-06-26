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

        /// <summary>
        /// Set Actor position based on roll pitch yaw
        /// </summary>
        /// <param name="X">X offset in degrees</param>
        /// <param name="Y">Y offset in degrees</param>
        /// <param name="Z">Z offset in degrees</param>
        public void SetActorPosition(float X, float Y, float Z)
        {
            m_Position.X = X;
            m_Position.Y = Y;
            m_Position.Z = Z;
        }

        /// <summary>
        /// Smooth translate an actor based on a vector offset
        /// </summary>
        /// <param name="Offset">Position offset</param>
        public void TranslateActor(Vector3 Offset)
        {
            m_Position.Plus(Offset);
        }
        
        /// <summary>
        /// Smooth translate an actor based on an x y z offset
        /// </summary>
        /// <param name="X">Offset</param>
        /// <param name="Y">Offset</param>
        /// <param name="Z">Offset</param>
        public void TranslateActor(float X, float Y, float Z)
        {
            m_Position.Plus(new Vector3(X, Y, Z));
        }

        public void RotateActorPitchYawRoll(Vector3 Offset)
        {
            m_Rotation.Plus(Offset);
        }

        public void RotateActorPitchYawRoll(float Pitch, float Yaw, float Roll)
        {
            m_Rotation.Plus(new Vector3(Pitch, Yaw, Roll));
        }

        public void ScaleActor(Vector3 Offset)
        {
            m_Scale.Plus(Offset);
        }

        public void ScaleActor(float X, float Y, float Z)
        {
            m_Scale.Plus(new Vector3(X, Y, Z));
        }

        public void SetActorScale(float X, float Y, float Z)
        {
            m_Scale.X = X;
            m_Scale.Y = Y;
            m_Scale.Z = Z;
        }

        public void SetActorRotation(float X, float Y, float Z)
        {
            m_Rotation.X = X;
            m_Rotation.Y = Y;
            m_Rotation.Z = Z;
        }
    }
}
