using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace RandomSeat
{
    class Student
    {
        private string m_Name;
        private string m_ID;

        public string Name
        {
            get { return m_Name; }
            set { m_Name = value; }
        }

        public string ID
        {
            get { return m_ID; }
            set { m_ID = value; }
        }

        public Student()
        {
            ID = "";
            Name = "";
        }
    }
}
