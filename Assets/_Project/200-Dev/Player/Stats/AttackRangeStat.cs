namespace Project
{
    public class AttackRangeStat : Stat<float>
    {
        public float AddRange(float amount)
        {
            float lastRange = value;
            
            value += amount;
            
            float newRange = value;
            
            return newRange - lastRange;
        }
    }
} 