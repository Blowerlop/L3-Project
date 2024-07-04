using System.Collections.Generic;
using UnityEngine;

namespace Project
{
    public class MoveFXs : MonoBehaviour
    {
        [SerializeField] private ParticlesCallback moveFXPrefab;
        [SerializeField] private Vector3 offset;
        
        private readonly Queue<ParticlesCallback> _pool = new();
        
        public void PlayFX(Vector3 position)
        {
            ParticleSystem fx = GetFX();
            
            fx.gameObject.SetActive(true);
            
            fx.transform.position = position + offset;
            fx.Play();
        }

        private ParticleSystem GetFX()
        {
            if (_pool.TryDequeue(out var result)) return result.ParticleSystem;

            ParticlesCallback newFX = Instantiate(moveFXPrefab);
            newFX.OnStopped += () =>
            {
                newFX.ParticleSystem.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
                newFX.gameObject.SetActive(false);
                
                _pool.Enqueue(newFX);
            };
            
            return newFX.ParticleSystem;
        }
    }
}