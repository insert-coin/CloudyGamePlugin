namespace FUdpRemoteControllerSegment {
	struct FKeyBoardChunk 
	{
		uint8 Version;
		uint8 InputType;
		uint32 Sequence;
		uint8 ControllerID;
		uint32 KeyCode;
		uint8 InputEvent;

		friend FArchive& operator<<(FArchive& Ar, FKeyBoardChunk& Chunk)
		{
			return Ar
				<< Chunk.Version
				<< Chunk.InputType
				<< Chunk.Sequence
				<< Chunk.ControllerID
				<< Chunk.KeyCode
				<< Chunk.InputEvent;
		}
	};
}