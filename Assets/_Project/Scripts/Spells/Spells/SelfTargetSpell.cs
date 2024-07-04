using Project._Project.Scripts.Managers;
using UnityEngine;

namespace Project.Spells
{
    public class SelfTargetSpell : Spell
    {
        [SerializeField] private bool followPlayer;
        [SerializeField] private float duration = 2f;
        
        protected override void Init(ICastResult castResult)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return;
            }

            TryApplyEffects(Caster.GetPC().Entity);

            StartCoroutine(Utilities.WaitForSecondsAndDoActionCoroutine(duration, KillSpell));
        }

        public override (Vector3, Quaternion) GetDefaultTransform(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return default;
            }
            
            return (player.PlayerTransform.position, Quaternion.identity);
        }

        public override Vector3 GetDirection(ICastResult castResult, PlayerRefs player)
        {
            if (castResult is not EmptyResults)
            {
                Debug.LogError(
                    $"Given channeling result {nameof(castResult)} is not the required type for {nameof(SelfTargetSpell)}!");
                return default;
            }
            
            return player.PlayerTransform.forward;
        }

        private void LateUpdate()
        {
            if (!IsOnServer) return;
            if (!followPlayer) return;
            
            transform.position = Caster.PlayerTransform.position;
        }

        public override void OnNetworkSpawn()
        {
            base.OnNetworkSpawn();
            
            SoundManager.instance.PlaySingleSound("inst_" + Data.spellId, gameObject, SoundManager.EventType.Spell);
        }
    }
}
