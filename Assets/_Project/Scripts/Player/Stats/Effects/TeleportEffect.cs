using Unity.Netcode.Components;
using UnityEngine;
using UnityEngine.AI;

namespace Project.Effects
{
    public class TeleportEffect : Effect
    {
        public override EffectType Type => EffectType.Neutral;
        protected override bool AddToEffectableList => false;

        [SerializeField] private float _distanceToTarget;
        
        protected override bool TryApply_Internal(IEffectable effectable, PlayerRefs applier, Vector3 applyPosition)
        {
            var dirToTarget = applyPosition - applier.PlayerTransform.position;
            dirToTarget.y = 0;
            
            dirToTarget -= dirToTarget.normalized * _distanceToTarget;

            var pos = applier.PlayerTransform.position + dirToTarget;
            int maxAttemps = 20;

            float distanceOffsetPerTry = 0.2f;

            var index = 0;
            NavMeshHit hit;
            
            while (!NavMesh.SamplePosition(pos, out hit, 0.5f, NavMesh.AllAreas) && index < maxAttemps)
            {
                var dir = Random.insideUnitCircle.normalized * (_distanceToTarget + distanceOffsetPerTry * index);
                var toV3 = new Vector3(dir.x, 0, dir.y);
                
                pos = applyPosition + toV3;
                index++;
            }
            
            if (index >= maxAttemps)
            {
                Debug.LogError("Couldn't find a valid position to teleport! Target might be outside of the navmesh.");
                return false;
            }
            
            applier.PlayerTransform.GetComponent<NetworkTransform>()
                .Teleport(hit.position, Quaternion.LookRotation(applyPosition - hit.position), Vector3.one);

            var navMeshAgent = applier.GetPC().NavMeshAgent;
            
            navMeshAgent.velocity = Vector3.zero;
            navMeshAgent.isStopped = true;
            navMeshAgent.ResetPath();
            
            navMeshAgent.Warp(hit.position);
            
            return true;
        }

        protected override void KillEffect_Internal() { }

        public override Effect GetInstance()
        {
            return this;
        }
    }
}