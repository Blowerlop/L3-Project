using System;
using Sirenix.Utilities;
using TMPro;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Project._Project.TESTT_REBIND
{
    public static class InputSettingsManager
    {
        private static readonly string _cancelBinding = "<Keyboard>/escape";

        public static Action<InputAction, int> onRebindStarted;
        public static Action onRebindComplete;
        public static Action onRebindCanceled;
    
    
        public static void StartRebind(string actionName, int bindingIndex, TMP_Text statusText, bool excludeMouse)
        {
            InputAction action = InputManager.instance.inputAction.asset.FindAction(actionName);
            if (action == null || action.bindings.Count <= bindingIndex)
            {
                Debug.Log("Couldn't find action or binding");
                return;
            }

            if (action.bindings[bindingIndex].isComposite)
            {
                int firstPartIndex = bindingIndex + 1;
                if (firstPartIndex < action.bindings.Count && action.bindings[firstPartIndex].isComposite)
                    DoRebind(action, bindingIndex, statusText, true, excludeMouse);
            }
            else
                DoRebind(action, bindingIndex, statusText, false, excludeMouse);
        }

        private static void DoRebind(InputAction actionToRebind, int bindingIndex, TMP_Text statusText, bool allCompositeParts, bool excludeMouse)
        {
            if (actionToRebind == null || bindingIndex < 0)
                return;

            // statusText.text = $"Press a {actionToRebind.expectedControlType}";
            statusText.text = $"Press a key";

            bool actionWasEnabled = actionToRebind.enabled;
            actionToRebind.Disable();

            InputActionRebindingExtensions.RebindingOperation rebind = actionToRebind
                .PerformInteractiveRebinding(bindingIndex)
                .WithCancelingThrough(_cancelBinding)
                .OnComplete(operation =>
                {
                    if (actionWasEnabled) actionToRebind.Enable();
                    operation.Dispose();

                    if (allCompositeParts)
                    {
                        var nextBindingIndex = bindingIndex + 1;
                        if (nextBindingIndex < actionToRebind.bindings.Count &&
                            actionToRebind.bindings[nextBindingIndex].isComposite)
                            DoRebind(actionToRebind, nextBindingIndex, statusText, true, excludeMouse);
                    }
                    
                    SaveBindingOverride(actionToRebind);
                    onRebindComplete?.Invoke();
                })
                .OnCancel(operation =>
                {
                    if (actionWasEnabled) actionToRebind.Enable();
                    operation.Dispose();

                    onRebindCanceled?.Invoke();
                });
            
            if (excludeMouse)
                rebind.WithControlsExcluding("Mouse");

            onRebindStarted?.Invoke(actionToRebind, bindingIndex);
            rebind.Start(); //actually starts the rebinding process
        }

        public static string GetBindingName(string actionName, int bindingIndex, InputBinding.DisplayStringOptions displayStringOptions = 0)
        {
            InputAction action = InputManager.instance.inputAction.asset.FindAction(actionName);
            return action.GetBindingDisplayString(bindingIndex, displayStringOptions);
        }

        private static void SaveBindingOverride(InputAction action)
        {
            for (int i = 0; i < action.bindings.Count; i++)
            {
                PlayerPrefs.SetString(action.actionMap + action.name + i, action.bindings[i].overridePath);
            }
        }

        public static void Load()
        {
            InputManager.instance.inputAction.asset.actionMaps
                .ForEach(actionMap => actionMap.actions
                    .ForEach(action => LoadBindingOverride(action.name)));
        }
        
        private static void LoadBindingOverride(string actionName)
        {
            InputAction action = InputManager.instance.inputAction.asset.FindAction(actionName);

            for (int i = 0; i < action.bindings.Count; i++)
            {
                if (!string.IsNullOrEmpty(PlayerPrefs.GetString(action.actionMap + action.name + i)))
                {
                    action.ApplyBindingOverride(i, PlayerPrefs.GetString(action.actionMap + action.name + i));
                }
            }
        }

        public static void ResetBinding(string actionName, int bindingIndex)
        {
            InputAction action = InputManager.instance.inputAction.asset.FindAction(actionName);

            if(action == null || action.bindings.Count <= bindingIndex)
            {
                Debug.Log("Could not find action or binding");
                return;
            }

            if (action.bindings[bindingIndex].isComposite)
            {
                for (int i = bindingIndex; i < action.bindings.Count && action.bindings[i].isComposite; i++)
                    action.RemoveBindingOverride(i);
            }
            else
                action.RemoveBindingOverride(bindingIndex);

            SaveBindingOverride(action);
        }        
        
        public static void ClearBindingOverride(string actionName, int bindingIndex)
        {
            InputAction action = InputManager.instance.inputAction.asset.FindAction(actionName);

            action.ApplyBindingOverride(bindingIndex, " ");
            
            SaveBindingOverride(action);
        }
    }
}
