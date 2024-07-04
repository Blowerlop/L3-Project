using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Project
{
    public class HealthTextPool : MonoSingleton<HealthTextPool>
    {
        [SerializeField] private HealthText healthTextPrefab;
        [SerializeField] private float timeBetweenRequests = 0.1f;
        
        private Queue<HealthText> _pool = new();
        
        private Queue<Request> _requests = new();
        
        private Coroutine _handleRequestsCoroutine;

        protected override void Awake()
        {
            dontDestroyOnLoad = false;
            base.Awake();
        }
        
        public void RequestText(int value, Transform parent, Vector3 direction)
        {
            _requests.Enqueue(new Request
            {
                value = value,
                parent = parent,
                direction = direction
            });
            
            if (_handleRequestsCoroutine != null) return;
            
            _handleRequestsCoroutine = StartCoroutine(HandleRequests());
        }
        
        private void ShowText(int value, Transform parent, Vector3 direction)
        {
            var result = _pool.Count > 0 ? _pool.Dequeue() : Instantiate(healthTextPrefab);
            
            result.Init(value, parent, direction);
            
            result.OnStopped += () =>
            {
                result.gameObject.SetActive(false);
                _pool.Enqueue(result);
            };
        }

        private IEnumerator HandleRequests()
        {
            while(_requests.Count > 0)
            {
                var request = _requests.Dequeue();
                ShowText(request.value, request.parent, request.direction);
                
                yield return new WaitForSeconds(timeBetweenRequests);
            }
            
            _handleRequestsCoroutine = null;
        }

        private struct Request
        {
            public int value;
            public Transform parent;
            public Vector3 direction;
        }
    }
}