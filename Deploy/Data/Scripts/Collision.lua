CollisionType = {}

CollisionType.None = 0
CollisionType.Player = bit.lshift(1, 0)
CollisionType.Enemy = bit.lshift(1, 1)
CollisionType.Projectile = bit.lshift(1, 2)
CollisionType.Pickup = bit.lshift(1, 3)
CollisionType.Environment = bit.lshift(1, 4)
