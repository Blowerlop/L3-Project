using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using _Project._200_Dev.Console;
using _Project._200_Dev.Utilities.Extensions;
using UnityEditor;
using UnityEngine;
using UnityEngine.InputSystem;

namespace _Project._200_Dev.Utilities
{
    public class Utilities : MonoBehaviour
    {
        // DeclaredOnly, Instance, Static, Public, NonPublic
        public const BindingFlags BINDING_FLAGS_DEBUG = (BindingFlags)62;
        
        public static IEnumerator LerpInTimeCoroutine(float timeInSeconds, float from, float to, Action<float> callback, Action onFinishCallback = null)
        {
            float timer = 0.0f;
            while (timer < timeInSeconds)
            {
                timer += Time.deltaTime;
                float linearValue = Mathf.Lerp(from, to, timer / timeInSeconds);
                callback.Invoke(linearValue);
                yield return null;
            }
            
            onFinishCallback?.Invoke();
        }
        
        public static IEnumerator LerpInTimeCoroutine(float timeInSeconds, Vector3 from, Vector3 to, Action<Vector3> callback, Action onFinishCallback = null)
        {
            float timer = 0.0f;
            while (timer < timeInSeconds)
            {
                timer += Time.deltaTime;
                Vector3 linearValue = Vector3.Lerp(from, to, timer / timeInSeconds);
                callback.Invoke(linearValue);
                yield return null;
            }
            
            onFinishCallback?.Invoke();
        }
        
        public static IEnumerator LerpInTimeCoroutine(float timeInSeconds, Quaternion from, Quaternion to, Action<Quaternion> callback, Action onFinishCallback = null)
        {
            float timer = 0.0f;
            while (timer < timeInSeconds)
            {
                timer += Time.deltaTime;
                Quaternion linearValue = Quaternion.Lerp(from, to, timer / timeInSeconds);
                callback.Invoke(linearValue);
                yield return null;
            }
            
            onFinishCallback?.Invoke();
        }

        public static void StartWaitForSecondsAndDoActionCoroutine(MonoBehaviour monoBehaviour, float timeInSeconds, Action action)
        {
            monoBehaviour.StartCoroutine(WaitForSecondsAndDoActionCoroutine(timeInSeconds, action));
        }
        
        public static IEnumerator WaitForSecondsAndDoActionCoroutine(float timeInSeconds, Action action)
        {
            yield return new WaitForSeconds(timeInSeconds);
            action.Invoke();
        }
        
        public static void StartWaitForFramesAndDoActionCoroutine(MonoBehaviour monoBehaviour, int frames, Action action)
        {
            monoBehaviour.StartCoroutine(WaitForFramesAndDoActionCoroutine(frames, action));
        }
        
        public static IEnumerator WaitForFramesAndDoActionCoroutine(int frames, Action action)
        {
            for (int i = 0; i < frames; i++)
            {
                yield return null;
            }
            
            action.Invoke();
        }

        public static void StartWaitForEndOfFrameAndDoActionCoroutine(MonoBehaviour monoBehaviour, Action action)
        {
            monoBehaviour.StartCoroutine(WaitForEndOfFrameAndDoActionCoroutine(action));
        }
        
        public static IEnumerator WaitForEndOfFrameAndDoActionCoroutine(Action action)
        {
            yield return new WaitForEndOfFrame();            
            action.Invoke();
        }

        public static IEnumerator WaitWhileAndDoAction(Func<bool> predicate, Action action)
        {
            yield return new WaitWhile(predicate);
            action.Invoke();
        }

        public static void StartWaitUntilAndDoAction(MonoBehaviour monoBehaviour, Func<bool> predicate, Action action)
        {
            monoBehaviour.StartCoroutine(WaitUntilAndDoAction(predicate, action));
        }
        
        public static IEnumerator WaitUntilAndDoAction(Func<bool> predicate, Action action)
        {
            yield return new WaitUntil(predicate);
            action.Invoke();
        }

        public static bool GetMouseWorldHit(Camera camera, LayerMask layerMask, out RaycastHit hitInfo)
        {
            Vector3 mousePosition = Mouse.current.position.value;
            mousePosition.z = camera.nearClipPlane;
            
            Ray ray = camera.ScreenPointToRay(mousePosition);
            return Physics.Raycast(ray, out hitInfo, 100, layerMask);
        }

        public static bool GetMouseWorldPosition(Camera camera, LayerMask layerMask, out Vector3 position)
        {
            Vector3 mousePosition = Mouse.current.position.value;
            mousePosition.z = camera.nearClipPlane;

            Ray ray = camera.ScreenPointToRay(mousePosition);
            if (Physics.Raycast(ray, out RaycastHit hitInfo,100, layerMask))
            {
                position = hitInfo.point;
                return true;
            }
            
            position = Vector3.zero;
            return false;
        }

        #if UNITY_EDITOR
        /// <summary>
        /// /!\ Editor only
        /// </summary>
        /// <param name="type"></param>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public static T[] FindAssetsByType<T>(Type type)  
        {
            string[] guids = AssetDatabase.FindAssets($"t:{type.Name}");
            
            return guids.Select(AssetDatabase.GUIDToAssetPath)
                        .Select(path => AssetDatabase.LoadAssetAtPath(path, type))
                        .Cast<T>()
                        .ToArray();
        }
        
        /// <summary>
        /// /!\ Editor only
        /// </summary>
        /// <param name="type"></param>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public static T[] FindAssetsByType<T>() where T : UnityEngine.Object 
        {
            string[] guids = AssetDatabase.FindAssets($"t:{typeof(T)}");
            
            return guids.Select(AssetDatabase.GUIDToAssetPath)
                        .Select(AssetDatabase.LoadAssetAtPath<T>)
                        .ToArray();
        }

        public static IEnumerable<Type> GetDomainTypes()
        {
            // return AppDomain.CurrentDomain.GetAssemblies().SelectMany(x => x.GetTypes()) ;
            return new[] { typeof(Utilities) };
        }
        #endif
        
        [ConsoleCommand("ui_disable", "Disable all UI elements in the scene.")]
        private static void DisableAllUi()
        {
            foreach (var canvas in FindObjectsByType<Canvas>(FindObjectsSortMode.None))
            {
                canvas.enabled = false;
            }
            
            Console.Console.instance.GetComponentsInChildren<Canvas>().ForEach(x => x.enabled = true);
        }
        
        [ConsoleCommand("ui_enable", "Enable all UI elements in the scene.")]
        private static void EnableAllUi()
        {
            foreach (var canvas in FindObjectsByType<Canvas>(FindObjectsSortMode.None))
            {
                canvas.enabled = true;
            }
        }
    }
}
