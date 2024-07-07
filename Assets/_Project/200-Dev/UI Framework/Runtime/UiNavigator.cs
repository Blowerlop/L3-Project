using System.Collections.Generic;
using System.Linq;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;

namespace Project.Scripts.UIFramework
{
    public class UiNavigator : MonoBehaviour, IPointerClickHandler
    {
        [ShowInInspector, ReadOnly] private static List<UiNavigable> _navigables = new List<UiNavigable>();

        
        private void Initialized()
        {
            CursorManager.Request(CursorLockMode.Confined, true);
            InputManager.instance.onEscapeKey.performed += CloseElement;
            if (InputManager.instance.currentActionMap.name != "UI") InputManager.instance.SwitchActionMap("UI");
        }

        private void UnInitialized()
        {
            CursorManager.Release();
            if (InputManager.IsInstanceAlive())
            {
                InputManager.instance.onEscapeKey.performed -= CloseElement;
                InputManager.instance.SwitchActionMap(InputManager.instance.previousActionMap);
            }
        }
        
        public void OnPointerClick(PointerEventData eventData)
        {
            CloseElement(default);
        }

        public static bool IsNavigating()
        {
            return _navigables.Any();
        }
        
        public void Register(UiNavigable element)
        {
            _navigables.Add(element);
            _navigables = _navigables.OrderBy(x => x.hierarchyDepth).ToList();

            
            if (_navigables.Count == 1) Initialized();
        }
        
        public void UnRegister(UiNavigable element)
        {
            _navigables.Remove(element);
            
            if (_navigables.Count == 0) UnInitialized();
        }

        private void CloseElement(InputAction.CallbackContext _)
        {
            if (IsNavigating() == false)
            {
                Debug.LogError("Trying to close an element where the is no more registered. This is a bug.");
                return;
            }

            _navigables[^1].Close();
        }
    }
}