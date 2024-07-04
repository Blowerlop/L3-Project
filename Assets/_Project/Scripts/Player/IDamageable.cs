namespace Project
{
    public interface IDamageable
    {
        /// <summary>
        /// This method doesn't check if the attacker can damage this damageable.
        /// Use <see cref="CanDamage"/> if you want to.
        /// </summary>
        /// <param name="modifier"></param>
        public void Damage(int modifier);

        public bool CanDamage(int teamIndex);

        /// <summary>
        /// Does the same as <see cref="Damage"/> but checks if the attacker can damage before applying.
        /// </summary>
        /// <param name="modifier"></param>
        /// <param name="attackerTeamIndex"></param>
        /// <returns>Damage are applied or not</returns>
        public bool TryDamage(int modifier, int attackerTeamIndex)
        {
            if (!CanDamage(attackerTeamIndex)) return false;
            
            Damage(modifier);
            return true;
        }
    }
}