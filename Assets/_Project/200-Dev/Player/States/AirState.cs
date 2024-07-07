using Project._Project.Scripts.StateMachine;

namespace Project._Project.Scripts.Player.State.PlayerState.Base.Air
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