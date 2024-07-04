using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.Events;
using Object = UnityEngine.Object;

public enum EAxis
{
    X,
    Y,
    Z
}

namespace Project.Extensions
{
    public static class TransformExtensions
    {
        public static List<Transform> GetChildrenFirstDepth(this Transform transform)
        {
            return GetComponentsInChildrenFirstDepthWithoutTheParent<Transform>(transform);
        }

        public static List<Transform> GetChildrenRecursively(this Transform transform, List<Transform> children = null)
        {
            return GetComponentsInChildrenWithoutParent<Transform>(transform);
        }

        public static List<T> GetComponentsInChildrenFirstDepthWithoutTheParent<T>(this Transform transform)
            where T : Object
        {
            List<T> children = new List<T>();

            for (int i = 0; i < transform.childCount; i++)
            {
                if (transform.GetChild(i).TryGetComponent(out T component))
                {
                    children.Add(component);
                }
            }

            return children;
        }

        public static List<T> GetComponentsInChildrenWithoutParent<T>(this Transform transform) where T : Object
        {
            return GetComponentsInChildrenWithoutParent(transform, new List<T>());
        }
        
        private static List<T> GetComponentsInChildrenWithoutParent<T>(this Transform transform,
            List<T> children) where T : Object
        {
            for (int i = 0; i < transform.childCount; i++)
            {
                Transform child = transform.GetChild(i);

                child.GetComponentsInChildrenWithoutParent(children);
                if (child.TryGetComponent(out T tChild))
                {
                    children.Add(tChild);
                }
            }

            return children;
        }
        
        public static void DestroyChildren(this GameObject gameObject)
        {
            gameObject.transform.DestroyChildren();
        }
        
        public static void DestroyChildren(this Transform transform)
        {
            for (int i = transform.childCount - 1; i >= 0; i--)
            {
                if (Application.isEditor)
                {
                    Object.DestroyImmediate(transform.GetChild(i).gameObject);
                }
                else
                {
                    Object.Destroy(transform.GetChild(i).gameObject);
                }
            }
        }
    }

    public static class RigidbodyExtensions
    {
        public static void ResetVelocities(this Rigidbody rigidbody)
        {
            rigidbody.linearVelocity = Vector3.zero;
            rigidbody.angularVelocity = Vector3.zero;
        }
    }

    public static class CollectionsExtensions
    {
        public static void ForEach<T>(this IList<T> target, Action<T> action)
        {
            for (int i = 0; i < target.Count; i++)
            {
                action.Invoke(target[i]);
            }
        }

        public static void ForEach<T>(this IList<T> target, Action<T, int> action)
        {
            for (int i = 0; i < target.Count; i++)
            {
                action.Invoke(target[i], i);
            }
        }

        public static void ForEach<T1, T2>(this Dictionary<T1, T2> target, Action<T1, T2> action)
        {
            foreach (KeyValuePair<T1, T2> kvp in target)
            {
                action.Invoke(kvp.Key, kvp.Value);
            }
        }

        public static void Debug<T>(this IList<T> target, string textToInsertBefore = "")
        {
            for (int i = 0; i < target.Count; i++)
            {
                UnityEngine.Debug.Log(textToInsertBefore + target[i]);
            }
        }
        
        public static IEnumerable<TSource> DistinctBy<TSource, TKey>(this IEnumerable<TSource> source,
            Func<TSource, TKey> keySelector)
        {
            return _(); IEnumerable<TSource> _()
            {
                var knownKeys = new HashSet<TKey>();
                foreach (var element in source)
                {
                    if (knownKeys.Add(keySelector(element)))
                        yield return element;
                }
            }
        }

        public static int FindIndex<T>(this T[] array, Predicate<T> match)
        {
            for (int i = 0; i < array.Length; i++)
            {
                if (match(array[i]))
                    return i;
            }

            return -1;
        }
    }

    public static class StringExtensions
    {
        public static string SeparateContent(this string text)
        {
            return string.Concat(text.Select(x => Char.IsUpper(x) ? " " + x : x.ToString())).TrimStart(' ');
        }
        
        public static string ExtractNumber(this string text)
        {
            var match = Regex.Match(text, @"([-+]?[0-9]*\.?[0-9]+)");
            if (match.Success)
                return (match.Groups[1].Value);

            return string.Empty;
        }
        
        public static string RemoveFirstLine(this string text)
        {
            return text.Substring(text.IndexOf("\n", StringComparison.Ordinal)+1);
        }
        
        public static string FollowCasePattern(this string text, string target)
        {
            int textLength = text.Length;
            if (textLength < target.Length) throw new ArgumentOutOfRangeException();
            
            StringBuilder stringBuilder = new StringBuilder();
            
            for (int i = 0; i < textLength; i++)
            {
                if (char.IsUpper(target[i]))
                {
                    stringBuilder.Append(char.ToUpper(text[i]));
                }
                else
                {
                    stringBuilder.Append(char.ToLower(text[i]));
                }
            }

            return stringBuilder.ToString();
        }
        
        public static int ToHashIsSameAlgoOnUnreal(this string stringToHash)
        {
            stringToHash = stringToHash.ToLower();
            
            const int p = 31;
            const long m = (long)1e9 + 9;
            long hashValue = 0;
            long pPow = 1;
            foreach (char c in stringToHash) {
                hashValue = (hashValue + (c - 'a' + 1) * pPow) % m;
                pPow = (pPow * p) % m;
            }
            return (int)hashValue; 
        }
        
        public static string ConvertToValidIdentifier(this string input, bool isPath = false)
        {
            if (isPath) input = Path.GetFileNameWithoutExtension(input);
            
            // Replace all invalid characters
            input = Regex.Replace(input, "[^a-zA-Z0-9_]", "_", RegexOptions.Compiled);
            if (input.EndsWith('_')) input = input.Remove(input.Length - 1);
            
            return input;
        }
    }

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

    public static class UnityObjectExtensions
    {
        public static T IsNull<T>(this T @object) where T : Object
        {
            return @object == null ? null : @object;
        }
    }

    public static class VectorExtensions
    {
        public static Vector3 ResetAxis(this Vector3 vector3, EAxis axis)
        {
            switch (axis)
            {
                case EAxis.X:
                    vector3.x = 0.0f;
                    break;
                
                case EAxis.Y:
                    vector3.y = 0.0f;
                    break;
                
                case EAxis.Z:
                    vector3.z = 0.0f;
                    break;
            }

            return vector3;
        }
    }

    public static class BooleanExtensions
    {
        public static int ToInt(this bool boolean) => boolean ? 1 : 0;

        public static bool ToBool(this int @int) => @int == 1 ? true : false;
    }

    public static class NetcodeExtensions
    {
        public static bool IsClientOnly(this NetworkBehaviour networkBehaviour)
        {
            return networkBehaviour.IsServer == false && networkBehaviour.IsClient;
        }
        
        public static bool IsServerOnly(this NetworkBehaviour networkBehaviour)
        {
            return networkBehaviour.IsServer && networkBehaviour.IsClient == false;
        }
    }
}
