using System.Collections.Generic;
using Sirenix.OdinInspector;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player
{

    
    public class EntityStatusFXHelper : MonoBehaviour
    {
        public enum PlayerStatusEffect
        {
            Silence,
            Stun,
            Fear,
        }
        [System.Serializable]
        private class StatusFXLinker
        {
            public PlayerStatusEffect statusEffect;
            public List<ParticleSystem> particleSystems; 
        }

        [SerializeField] Entity entity;

        [SerializeField] private List<StatusFXLinker> links = new List<StatusFXLinker>();
        private Dictionary<PlayerStatusEffect, List<ParticleSystem>> FXdict = new Dictionary<PlayerStatusEffect, List<ParticleSystem>>();

        private void Start()
        {
            for (int i = 0; i < links.Count; i++)
            {
                if (FXdict.ContainsKey(links[i].statusEffect))
                    Debug.LogError("There is same effect in fx list " + links[i].statusEffect);
                else FXdict.Add(links[i].statusEffect, links[i].particleSystems);

            }
            entity.OnSilenceChanged += SetSilenceFX;
        }

        private void OnDestroy()
        {
            entity.OnSilenceChanged -= SetSilenceFX;
        }


        [Button]
        void SetSilenceFX(bool isSilenced)
        {
            if(isSilenced)
            {
                for(int i = 0; i < FXdict[PlayerStatusEffect.Silence].Count; i++)
                {
                    FXdict[PlayerStatusEffect.Silence][i].Play();
                }
            }
            else
            {
                for (int i = 0; i < FXdict[PlayerStatusEffect.Silence].Count; i++)
                {
                    FXdict[PlayerStatusEffect.Silence][i].Stop(); FXdict[PlayerStatusEffect.Silence][i].Clear();
                }
            }
        }

        //Played with Animator FX Handler, No entity parameter
        void SetStunFX(bool isStunned)
        {

        }
    }
}
