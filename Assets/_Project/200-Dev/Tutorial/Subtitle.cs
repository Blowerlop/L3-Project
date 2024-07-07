using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using UnityEngine;

namespace _Project._200_Dev.Tutorial
{
    public static class Subtitle
    {
        // This regex will split the text into phrases, keeping the punctuation at the end of each phrase.
        // TODO: Fix regex keeping spaces at the beginning of each next phrase.
        private static readonly Regex _PunctuationRegex = new Regex(@"(?<=[.;!?])");

        private static Queue<string> _phrasesQueue = new();
        private static string _currentPhrase;

        public static event Action<string> OnWrite;

        // Settings
        private const float _AVERAGE_SECONDS_PER_WORD = 0.4f;
        private const int _MIN_DELAY_BETWEEN_PHRASES_IN_SECONDS = 1;

        private static Coroutine _coroutine;
        private static Action _storedCallback;

        /// Tu peux transformer les coroutines en UniTask stv.
        /// Ou bien si tu veux garder les coroutines, tu peux faire un GameObject singleton vide juste pour pouvoir déclencher les coroutines.


        public static void StartWriting(string speaker, string text, Action callback = null)
        {
            if (_coroutine != null)
            {
                _storedCallback?.Invoke();
                TutorialSequencer.instance.StopCoroutine(_coroutine);
                Debug.LogError("A new subtitle has started before the previous one finished.");
            }

            _coroutine = TutorialSequencer.instance.StartCoroutine(Write(speaker, _PunctuationRegex.Split(text).Where(t => t.Length != 0), callback));
            Debug.Log($"Subtitle started: {speaker} - {text}");
        }

        private static IEnumerator Write(string speaker, IEnumerable<string> phrases, Action callback)
        {
            _phrasesQueue = new Queue<string>(phrases);
            _storedCallback = callback;

            while (_phrasesQueue.Count > 0)
            {
                Next(speaker);

                int phraseTime = (int)(Count(_currentPhrase, ' ') * _AVERAGE_SECONDS_PER_WORD);
                yield return new WaitForSecondsRealtime(phraseTime + _MIN_DELAY_BETWEEN_PHRASES_IN_SECONDS);
            }

            _storedCallback?.Invoke();
            _coroutine = null;
        }

        private static void Next(string speaker)
        {
            SetText(speaker, _phrasesQueue.TryDequeue(out string phrase) ? phrase.Trim() : null);
        }

        private static void SetText(string speaker, string text)
        {
            _currentPhrase = string.IsNullOrEmpty(text) ? string.Empty : text;
            OnWrite?.Invoke(_currentPhrase);
        }

        private static int Count(string str, char separator)
        {
            int count = 0;

            // We don't count the first and last character because it's not possible to have a word at 0 or str.Length - 1
            // ---------------------------------
            //         ,Hello world,
            //  [0] <= ,           , => [str.Length - 1]
            // ---------------------------------
            for (int i = 1; i < str.Length - 1; i++)
            {
                if (str[i] == separator) count++;
            }

            return count;
        }
    }
}