using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

namespace Project
{
    public class AnimationEventHandler : MonoBehaviour
    {
        [Serializable]
        private struct Wrapper
        {
            public string id;
            public UnityEvent unityEvent;
        }
        
        [SerializeField] private List<Wrapper> wrappers = new();
        
        private Dictionary<string, UnityEvent> _events  = new();

        private void Awake()
        {
            foreach (var wrapper in wrappers)
            {
                if (_events.ContainsKey(wrapper.id))
                {
                    Debug.LogError($"Can't add AnimationEvent {wrapper.id} because it is already registered.");
                    continue;
                }
                
                _events.Add(wrapper.id, wrapper.unityEvent);
            }
        }

        // Had to change the name of the method because NetworkAnimator throw an error
        // --> 'Unity Failed to call AnimationEvent Invoke of class NetworkAnimator.
        // The function must have either 0 or 1 parameters and the parameter
        // can only be: string, float, int, enum, Object and AnimationEvent."
        //
        // Maybe related to https://forum.unity.com/threads/animation-events-problems.723623/
        public void Callback(string id)
        {
            if (!_events.ContainsKey(id))
            {
                Debug.LogError($"Can't invoke AnimationEvent {id} because it is not registered.");
                return;
            }
            
            _events[id]?.Invoke();
        }
    }
}
