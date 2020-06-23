using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace Internal
{
    class Vector3
    {
        public Vector3() { }
        public Vector3(float x, float y, float z) { m_X = x; m_Y = y; m_Z = z; }
        public Vector3(float vectorValue) { m_X = vectorValue; m_Y = vectorValue; m_Z = vectorValue; }

        public override string ToString() { return "x: " + m_X + ", y: " + m_Y + ", z: " + m_Z; }

        float m_X = 0.0f;
        float m_Y;
        float m_Z = 0.0f;
        
        public float X { get { return m_X; } set { m_X = value; } }
        public float Y { get { return m_Y; } set { m_Y = value; } }
        public float Z { get { return m_Z; } set { m_Z = value; } }

        public static Vector3 Up { get { return new Vector3(0.0f, 1.0f, 0.0f); } }
        public static Vector3 Down { get { return new Vector3(0.0f, -1.0f, 0.0f); } }
        public static Vector3 Left { get { return new Vector3(-1.0f, 0.0f, 0.0f); } }
        public static Vector3 Right { get { return new Vector3(1.0f, 0.0f, 0.0f); } }
        public static Vector3 Forwad { get { return new Vector3(0.0f, 0.0f, 1.0f); } }
        public static Vector3 Backward { get { return new Vector3(0.0f, 0.0f, -1.0f); } }

        public void Plus(Vector3 rhs)
        {
            m_X += rhs.X;
            m_Y += rhs.Y;
            m_Z += rhs.Z;
        }
        public void Minus(Vector3 rhs)
        {
            m_X -= rhs.X;
            m_Y -= rhs.Y;
            m_Z -= rhs.Z;
        }

        public double Length()
        {
            return Math.Sqrt(Math.Pow(m_X, 2) + Math.Pow(m_Y, 2) + Math.Pow(m_Z, 2));
        }

    }
}
