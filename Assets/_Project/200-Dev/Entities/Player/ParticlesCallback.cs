using System;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player
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