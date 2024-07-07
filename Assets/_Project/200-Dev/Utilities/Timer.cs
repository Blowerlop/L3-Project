using System;
using System.Collections;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project
{
    public enum TimeType
    {
        Scaled,
        Unscaled
    }
    public class Timer
    {
        [ShowInInspector] public float timer { get; private set; }
        public bool isTimerRunning => _timerCoroutine != null;
        private Coroutine _timerCoroutine;
        private MonoBehaviour _monoBehaviour;

        public void StartSimpleTimerScaled(MonoBehaviour monoBehaviour, float timeInSeconds, TimeType timerType = TimeType.Scaled, bool forceStart = false) =>
            StartTimerWithCallback(monoBehaviour, timeInSeconds, null, timerType, forceStart);
        
        public void StartTimerWithCallback(MonoBehaviour monoBehaviour, float timeInSeconds, Action callback, TimeType timerType = TimeType.Scaled, bool forceStart = false)
        {
            if (isTimerRunning)
            {
                if (forceStart)
                {
                    StopTimer();
                    return;
                }
                
                Debug.Log("A timer is already in progress");
                return;
            }

            _monoBehaviour = monoBehaviour;
            _timerCoroutine = monoBehaviour.StartCoroutine(TimerCoroutine(timeInSeconds, timerType, callback));
        }

        public void StartTimerWithUpdateCallback(MonoBehaviour monoBehaviour, float timeInSeconds,
            Action<float> updateCallback, Action callback = null, TimeType timerType = TimeType.Scaled,
            bool ceiled = false, bool forceStart = false)
        {
            if (isTimerRunning)
            {
                if (forceStart)
                {
                    StopTimer();
                    return;
                }

                Debug.Log("A timer is already in progress " + GetTimeRemaining());
                return;
            }

            _monoBehaviour = monoBehaviour;
            _timerCoroutine =
                monoBehaviour.StartCoroutine(TimerCoroutine(timeInSeconds, timerType, callback, updateCallback, ceiled));
        }

        private IEnumerator TimerCoroutine(float timeInSeconds, TimeType timerType, Action callback, Action<float> updateCallback = null, bool ceiled = false) 
        {
            timer = timeInSeconds;

            int lastSecond = Mathf.RoundToInt(timer);
            
            if (timerType == TimeType.Scaled)
            {
                while (timer > 0.0f)
                {
                    timer -= Time.deltaTime;

                    if (ceiled)
                    {
                        var round = Mathf.CeilToInt(timer);
                        if (round != lastSecond)
                        {
                            lastSecond = round;
                            updateCallback?.Invoke(round);
                        }
                    }
                    else updateCallback?.Invoke(timer);
                    yield return null;
                }
            }
            else
            {
                while (timer > 0.0f)
                {
                    timer -= Time.unscaledDeltaTime;
                    
                    if (ceiled)
                    {
                        var round = Mathf.CeilToInt(timer);
                        if (round != lastSecond)
                        {
                            lastSecond = round;
                            updateCallback?.Invoke(round);
                        }
                    }
                    else updateCallback?.Invoke(timer);
                    yield return null;
                }
            }
            
            _timerCoroutine = null;
            _monoBehaviour = null;
            
            callback?.Invoke();
        }

        public void StopTimer()
        {
            if (_monoBehaviour != null && _timerCoroutine != null)
            {
                _monoBehaviour.StopCoroutine(_timerCoroutine);
                _timerCoroutine = null;
                _monoBehaviour = null;
            }
        }

        public float GetTimeRemaining() => timer;


        
        // -------------------------------------------------------------------------------------------------------------
        // Static Section

        public static Coroutine StartTimerWithCallbackScaled(MonoBehaviour monoBehaviour, float timeInSeconds, Action callback)
        {
            return monoBehaviour.StartCoroutine(TimerWithCallbackScaledCoroutine(timeInSeconds, callback));
        }
        
        public static Coroutine StartTimerWithCallbackUnscaled(MonoBehaviour monoBehaviour, float timeInSeconds, Action callback)
        {
            return monoBehaviour.StartCoroutine(TimerWithCallbackUnscaledCoroutine(timeInSeconds, callback));
        }

        private static IEnumerator TimerWithCallbackScaledCoroutine(float timeInSeconds, Action callback)
        {
            yield return new WaitForSeconds(timeInSeconds);
            callback?.Invoke();
        }
        
        private static IEnumerator TimerWithCallbackUnscaledCoroutine(float timeInSeconds, Action callback)
        {
            yield return new WaitForSecondsRealtime(timeInSeconds);
            callback?.Invoke();
        }
    }
}
