using System;
using System.Reflection;
using Sirenix.OdinInspector;

namespace _Project._200_Dev.Console
{
    [Serializable]
    public class ConsoleCommand
    {
        [ShowInInspector] public string name { get; private set; }
        public ParameterInfo[] parametersInfo { get; private set; }
        private MethodInfo _methodInfo;
        public uint parametersWithDefaultValue { get; private set; }
        public string description { get; private set; }
        

        private ConsoleCommand(string name, string description)
        {
            // this.name = RefactorNameByTemplate(name);
            this.name = name;
            this.description = description;
        }
        
        public ConsoleCommand(string name, string description, Action method) : this(name, description)
        {
            SetupFinalParameters(method.GetMethodInfo());
        }

        public ConsoleCommand(string name, string description, MethodInfo methodInfo) : this(name, description)
        {
            SetupFinalParameters(methodInfo);
        }


        /// <summary>
        /// KillPlayer => kill_player
        /// </summary>
        /// <param name="name"></param>
        // private string RefactorNameByTemplate(string name)
        // {
        //     // Split by Upper character and join them with _
        //     return string.Join('_', Regex.Split(name, @"(?<!^)(?=[A-Z])")).ToLower();
        // }
        

        private void SetupFinalParameters(MethodInfo methodInfo)
        {
            _methodInfo = methodInfo;
            parametersInfo = methodInfo.GetParameters();
            HasParametersInfoHaveDefaultValue();
            
            // #if UNITY_EDITOR || DEVELOPMENT_BUILD
            // description += $" ({_methodInfo.ReflectedType})";
            // #endif
        }

        public void InvokeMethod(object[] parameters)
        {
            _methodInfo.Invoke(null, parameters);
        }
        
        private void HasParametersInfoHaveDefaultValue()
        {
            for (int i = 0; i < parametersInfo.Length; i++)
            {
                if (parametersInfo[i].HasDefaultValue)
                {
                    parametersWithDefaultValue++;
                }
            }
        }

        public override string ToString()
        {
            return $"{name} --- {description}";
        }
    }
}
