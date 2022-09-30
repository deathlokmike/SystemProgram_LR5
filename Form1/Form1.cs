using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

namespace Form1
{
    public partial class Form1 : Form
    {
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] private static extern void SendString(string str, int threadNumber);
        [DllImport("MFCLibrary1.dll")] public static extern int SendInt(int n);
        [DllImport("MFCLibrary1.dll")] public static extern int GetInt();
        [DllImport("MFCLibrary1.dll")] private static extern void connectToServer();
        [DllImport("MFCLibrary1.dll")] private static extern void disconnect();
        public Form1()
        {
            InitializeComponent();
        }

        private void add_child()
        {
            if (treeView1.Nodes.Count == 1)
            {
                treeView1.Nodes.Add("Все потоки");
            }
            treeView1.Nodes[1].Nodes.Add("Поток " + (treeView1.Nodes[1].Nodes.Count + 1).ToString());
        }

        private void treeView1_AfterCheck(object sender, TreeViewEventArgs e)
        {
            foreach (TreeNode child in e.Node.Nodes)
            {
                child.Checked = e.Node.Checked;
            }
        }
        
        private List<int> get_current_num() // выдает значения по выбранным потоком
        {
            List<int> buff = new List<int>();
            if (treeView1.Nodes.Count > 0)
            {
                if (treeView1.Nodes[0].Checked)
                    buff.Add(-2);
                if (treeView1.Nodes.Count > 1)
                {
                    if (treeView1.Nodes[1].Checked)
                        buff.Add(-1);
                    else
                    {
                        int j = 0;
                        foreach (TreeNode child in treeView1.Nodes[1].Nodes)
                        {
                            if (child.Checked)
                                buff.Add(j);
                            j += 1;
                        }
                    }    
                }
            }
            return buff;
        }
        private void rebuildForm(int threadsNumber)
        {
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add("Главный поток");
            if (treeView1.Nodes.Count == 1)
            {
                treeView1.Nodes.Add("Все потоки");
            }
            for (int i = 0; i < threadsNumber; i++)
            {
                treeView1.Nodes[1].Nodes.Add("Поток " + (treeView1.Nodes[1].Nodes.Count + 1).ToString());
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            connectToServer();
            int maxThreads = Convert.ToInt32(textBox1.Text);
            SendInt(0);
            SendInt(maxThreads);
            rebuildForm(GetInt());
            disconnect();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (treeView1.Nodes.Count != 1)
            {
                connectToServer();
                SendInt(1);
                rebuildForm(GetInt());
                disconnect();
            }
            else
            {

            }
           /* if (console())
            {
                Send(1, "");
                if (treeView1.Nodes.Count != 1)
                {
                    if (treeView1.Nodes[1].Nodes.Count > 1)
                        treeView1.Nodes[1].Nodes.RemoveAt(treeView1.Nodes[1].Nodes.Count - 1);
                    else
                        treeView1.Nodes.RemoveAt(1);
                }
                else
                {
                    treeView1.Nodes.RemoveAt(0);
                }
                waitEventConfirm();
            }
            else
            {
                Send(2, "");
                waitEventConfirm();
                //Child = null;
                return;
            }*/
        }
        private void CloseApp(object sender, FormClosingEventArgs e)
        {
           
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            connectToServer();
            SendInt(20);
            rebuildForm(GetInt());
            disconnect();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            connectToServer();
            SendInt(2);
            string text = textBox2.Text;
            List<int> thread_nums = get_current_num();
            if (text.Length == 0 || thread_nums.Count == 0) return;
            foreach (int threadNumber in thread_nums)
            {
                SendString(text, threadNumber);
                rebuildForm(GetInt());
                disconnect();
            }
            
        }
    }
}
