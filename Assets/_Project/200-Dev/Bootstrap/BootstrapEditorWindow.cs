#if UNITY_EDITOR
using Sirenix.OdinInspector;
using Sirenix.OdinInspector.Editor;
using UnityEditor;
using UnityEngine;

namespace Project
{
    public class BootstrapEditorWindow : OdinEditorWindow
    {
        private static SOBootstrap _bootstrap;
        
        [EnumToggleButtons, OnValueChanged("GetValues")] public RuntimeInitializeLoadType runtimeInitializeLoadType;
        [OnValueChanged("SetValues")] public Object[] values;


        [MenuItem("Tools/Bootstrap")]
        public static void ShowWindow()
        {
            GetWindow<BootstrapEditorWindow>()?.Show();
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            
            _bootstrap = Bootstrapper.instance;
        }

        
        private void GetValues()
        {
            values = _bootstrap.GetObjects(runtimeInitializeLoadType);
        }

        private void SetValues()
        {
            _bootstrap.SetObjects(runtimeInitializeLoadType, values);
        }
    }
}
#endif