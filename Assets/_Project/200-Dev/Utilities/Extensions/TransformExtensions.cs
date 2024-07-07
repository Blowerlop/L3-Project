using System.Collections.Generic;
using UnityEngine;

namespace _Project._200_Dev.Utilities.Extensions
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
}