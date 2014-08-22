StateMachine = {}
StateMachine.__index = StateMachine

function StateMachine:New()
    local self = {}
    self.current = nil
    
    return setmetatable(self, StateMachine)
end

function StateMachine:Change(state)
    -- Exit current state.
    self:Call("OnExit")
    
    -- Enter new state.
    self.current = state
    self:Call("OnEnter")
end

function StateMachine:Call(name, ...)
    -- Try to call the state function and handle nil.
    if self.current then
        local reference = self.current[name]
        
        if reference then
            return reference(self.current, ...)
        end
    end
    
    return nil
end

function StateMachine:GetCurrent()
    return self.current
end

setmetatable(StateMachine, { __call = StateMachine.New })
