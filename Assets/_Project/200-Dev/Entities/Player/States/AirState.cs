using _Project._200_Dev.Entities.StateMachine;

namespace _Project._200_Dev.Entities.Player.States
{
    public class AirState : BaseStateMachineBehaviour //TODO: maybe with buff????
    {
        public override bool CanChangeStateTo<T>()
        {
            return true;
        }

        public override bool CanEnterState(PCPlayerRefs refs)
        {
            return true;
        }

        public override string ToString()
        {
            return "Air";
        }
    }
}