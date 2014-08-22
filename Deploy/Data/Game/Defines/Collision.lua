CollisionTypes = {}

CollisionTypes.None = 0
CollisionTypes.All = -1
CollisionTypes.Player = bit.lshift(1, 0)
CollisionTypes.Enemy = bit.lshift(1, 1)
CollisionTypes.Projectile = bit.lshift(1, 2)
CollisionTypes.Pickup = bit.lshift(1, 3)
CollisionTypes.Environment = bit.lshift(1, 4)
