using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.Events;


namespace Project.Scripts.UIFramework
{
    public class UiNavigable : MonoBehaviour
    {
        [SerializeField, Required] private UiNavigator _navigator;
        [field: SerializeField, ReadOnly] public uint hierarchyDepth { get; private set; }
        [SerializeField] private UnityEvent _onShow = new UnityEvent();
        [SerializeField] private UnityEvent _onHide = new UnityEvent();
        
        
        private void OnEnable()
        {
            _navigator.Register(this);
        }
        
        private void OnDisable()
        {
            _navigator.UnRegister(this);
        }

        private void OnValidate()
        {
            hierarchyDepth = GetHierarchyDepth();
        }
        

        public void Close()
        {
            gameObject.SetActive(false);
        }

        private uint GetHierarchyDepth()
        {
            uint depth = 0;
            
            Transform parent = transform.parent;
            while (parent != null)
            {
                depth++;
                parent = parent.parent;
            }

            return depth;
        }
    }
}