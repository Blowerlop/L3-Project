using System;
using System.Collections.Generic;

namespace _Project._200_Dev.Utilities.Extensions
{
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
}