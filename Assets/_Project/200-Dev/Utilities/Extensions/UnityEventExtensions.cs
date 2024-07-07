using System;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using UnityEngine.Events;
using Object = UnityEngine.Object;

namespace _Project._200_Dev.Utilities.Extensions
{
    public static class UnityEventExtensions
    {
        /// <summary>
        /// This extension will permit to add automatically a persistant listener when in editor and add a non-persistant listener when non in editor
        /// </summary>
        public static void AddListenerExtended(this UnityEvent unityEvent, UnityAction call)
        {
#if UNITY_EDITOR
            UnityEditor.Events.UnityEventTools.AddPersistentListener(unityEvent, call);
#else
                    unityEvent.AddListener(call);
#endif
        }

        /// <summary>
        /// This extension will permit to remove automatically a persistant listener when in editor and add a non-persistant listener when non in editor
        /// </summary>
        public static void RemoveListenerExtended(this UnityEvent unityEvent, UnityAction call)
        {
#if UNITY_EDITOR
            UnityEditor.Events.UnityEventTools.RemovePersistentListener(unityEvent, call);
#else
                    unityEvent.RemoveListener(call);
#endif
        }

        /// <summary>
        /// This extension will permit to add automatically a persistant listener when in editor and add a non-persistant listener when non in editor
        /// </summary>
        public static void AddListenerExtended<T>(this UnityEvent<T> unityEvent, UnityAction<T> call)
        {
#if UNITY_EDITOR
            UnityEditor.Events.UnityEventTools.AddPersistentListener<T>(unityEvent, call);
#else
                    unityEvent.AddListener(call);
#endif
        }

        /// <summary>
        /// This extension will permit to remove automatically a persistant listener when in editor and add a non-persistant listener when non in editor
        /// </summary>
        public static void RemoveListenerExtended<T>(this UnityEvent<T> unityEvent, UnityAction<T> call)
        {
#if UNITY_EDITOR
            UnityEditor.Events.UnityEventTools.RemovePersistentListener(unityEvent, call);
#else
                    unityEvent.RemoveListener(call);
#endif
        }


        public static List<UnityAction> CopyPersistentEvents(this UnityEvent source)
        {
            List<UnityAction> actions = new List<UnityAction>();
            
            for (int i = 0; i < source.GetPersistentEventCount(); i++)
            {
                Object target = source.GetPersistentTarget(i);
                string methodName = source.GetPersistentMethodName(i);
                Debug.Log("Target: " + target + " MethodName: " + methodName);

                MethodInfo methodInfo = UnityEventBase.GetValidMethodInfo(target, methodName, null);
                
                UnityAction execute = Delegate.CreateDelegate(typeof(UnityAction), methodInfo) as UnityAction;
                 
                actions.Add(execute);
            }

            return actions;
        }
        
        public static void PastePersistentEvents(this UnityEvent target, List<UnityAction> actions)
        {
            for (int i = 0; i < actions.Count; i++)
            {
                target.AddListenerExtended(actions[i]);
            }
            
            for (int i = 0; i < actions.Count; i++)
            {
                // if (target.GetPersistentEventCount() == 0)
                // {
                //     UnityEditor.Events.UnityEventTools.AddVoidPersistentListener(target, events[i]);
                //     UnityEditor.SceneManagement.EditorSceneManager.MarkSceneDirty(target.gameObject.scene);
                // }
                // else
                // {
                //     UnityEditor.Events.UnityEventTools.RegisterVoidPersistentListener(target, 0, events[i]);
                //     UnityEditor.SceneManagement.EditorSceneManager.MarkSceneDirty(target.gameObject.scene);
                // }
            }
        }
        
        public static void CopyPastePersistentEvents(this UnityEvent from, UnityEvent to)
        {
            to.PastePersistentEvents(from.CopyPersistentEvents());
        }
    }
}