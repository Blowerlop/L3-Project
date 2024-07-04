using System;
using UnityEngine;

namespace Project
{
    public class ParticlesCallback : MonoBehaviour
    {
        [field: SerializeField] public ParticleSystem ParticleSystem { get; private set; }
        
        public event Action OnStopped;
        
        private void OnParticleSystemStopped()
        {
            OnStopped?.Invoke();
        }
    }
}