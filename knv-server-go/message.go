package main

import (
	"encoding/json"
	"strconv"
)

const (
	MsgidLogin              = 1
	MsgidExit               = 2
	MsgidHash               = 3
	MsgidRestart            = 4
	MsgidRequestHash        = 5
	MsgidSuccessfulConnect  = 201
	MsgidMapUpload          = 202
	MsgidNewTick            = 203
	MsgidNewClient          = 204
	MsgidCurrentConnections = 205

	MsgidWrongGameVersion = 401
	MsgidWrongAuth        = 402
	MsgidUndefinedError   = 403
	MsgidServerExit       = 404
	MsgidNoMaster         = 405
	MsgidOutOfSync        = 406

	MsgidOrdinary    = 1001
	MsgidJustMessage = 1002
	MsgidGUI         = 1003
	MsgidMouseClick  = 1004
	MsgidOOCMessage  = 1005
	MsgidPing        = 1102
)

var maxMessageLength = map[uint32]int{
	MsgidLogin: 512,
	MsgidHash:  512,

	MsgidWrongGameVersion: 512,

	MsgidSuccessfulConnect:  512,
	MsgidMapUpload:          512,
	MsgidNewClient:          128,
	MsgidCurrentConnections: 128,

	MsgidOrdinary:    512,
	MsgidJustMessage: 2048,
	MsgidMouseClick:  512,
	MsgidOOCMessage:  2048,
	MsgidPing:        512,
}

type Message interface {
	TypeName() string
}

type Forwardable interface {
	SetID(int)
}

type Envelope struct {
	Message
	Kind uint32
	From int
}

var (
	emptyMessage = &MessageEmpty{}
)

func getConcreteMessage(id uint32) Message {
	switch {
	case id == MsgidLogin:
		return &MessageLogin{}
	case id == MsgidHash:
		return &MessageHash{}

	case id == MsgidSuccessfulConnect:
		return &MessageSuccessfulConnect{}
	case id == MsgidMapUpload:
		return &MessageMapUpload{}
	case id == MsgidNewTick:
		return &MessageNewTick{}
	case id == MsgidNewClient:
		return &MessageNewClient{}
	case id == MsgidCurrentConnections:
		return &MessageCurrentConnections{}
	case id == MsgidRequestHash:
		return &MessageRequestHash{}

		// errors
	case id == MsgidWrongGameVersion:
		return &ErrmsgWrongGameVersion{}
	case id == MsgidWrongAuth:
		return &ErrmsgWrongAuth{}
	case id == MsgidUndefinedError:
		return &ErrmsgUndefinedError{}
	case id == MsgidServerExit:
		return &ErrmsgServerExit{}
	case id == MsgidNoMaster:
		return &ErrmsgNoMaster{}
	case id == MsgidOutOfSync:
		return &ErrmsgOutOfSync{}

	case id == MsgidOrdinary:
		return &MessageOrdinary{}
	case id == MsgidJustMessage:
		return &MessageJustMessage{}
	case id == MsgidMouseClick:
		return &MessageMouseClick{}
	case id == MsgidOOCMessage:
		return &MessageOOC{}
	case id == MsgidPing:
		return &MessagePing{}

	case id >= 1000:
		m := OpaqueMessage(make(map[string]*json.RawMessage))
		return &m
	}
	return nil
}

type MessageIDEmbed struct {
	ID *int `json:"id"`
}

func (m *MessageIDEmbed) SetID(id int) {
	m.ID = &id
}

type MessageEmpty struct{}

func (m *MessageEmpty) TypeName() string {
	return "<empty>"
}

type MessageInput struct {
	MessageIDEmbed
	Key string `json:"key" validate:"nonzero"`
}

func (m *MessageInput) TypeName() string {
	return "MessageInput"
}

type MessageChat struct {
	MessageIDEmbed
	Type string `json:"type" validate:"nonzero"`
	Text string `json:"text" validate:"nonzero"`
}

func (m *MessageChat) TypeName() string {
	return "MessageChat"
}

type MessageLogin struct {
	Login       string `json:"login" validate:"nonzero"`
	Password    string `json:"password"`
	IsGuest     bool   `json:"guest"`
	GameVersion string `json:"game_version" validate:"nonzero"`
}

func (m *MessageLogin) TypeName() string {
	return "MessageLogin"
}

type MessageHash struct {
	Hash *int `json:"hash" validate:"nonzero"`
	Tick *int `json:"tick" validate:"nonzero"`
}

func (m *MessageHash) TypeName() string {
	return "MessageHash"
}

type MessageRequestHash struct {
	Tick *int `json:"tick" validate:"nonzero"`
}

func (m *MessageRequestHash) TypeName() string {
	return "MessageRequestHash"
}

type MessageSuccessfulConnect struct {
	ID     *int   `json:"your_id" validate:"nonzero"`
	MapURL string `json:"map" validate:"nonzero"`
}

func (m *MessageSuccessfulConnect) TypeName() string {
	return "MessageSuccessfulConnect"
}

type MessageMapUpload struct {
	Tick   *int   `json:"tick" validate:"nonzero"`
	MapURL string `json:"url_to_upload_map" validate:"nonzero"`
}

func (m *MessageMapUpload) TypeName() string {
	return "MessageMapUpload"
}

type MessageNewTick struct{}

func (m *MessageNewTick) TypeName() string {
	return "MessageNewTick"
}

type MessageNewClient struct {
	ID *int `json:"id" validate:"nonzero"`
}

func (m *MessageNewClient) TypeName() string {
	return "MessageNewClient"
}

type MessageCurrentConnections struct {
	Amount *int `json:"amount" validate:"nonzero"`
}

func (m *MessageCurrentConnections) TypeName() string {
	return "MessageCurrentConnections"
}

// errors
type ErrmsgWrongGameVersion struct {
	CorrectVersion string `json:"correct_game_version" validate:"nonzero"`
}

func (m *ErrmsgWrongGameVersion) TypeName() string {
	return "ErrmsgWrongGameVersion"
}

type ErrmsgWrongAuth struct {
}

func (m *ErrmsgWrongAuth) TypeName() string {
	return "ErrmsgWrongAuth"
}

type ErrmsgServerExit struct {
}

func (m *ErrmsgServerExit) TypeName() string {
	return "ErrmsgServerExit"
}

type ErrmsgNoMaster struct {
}

func (m *ErrmsgNoMaster) TypeName() string {
	return "ErrmsgNoMaster"
}

type ErrmsgOutOfSync struct {
}

func (m *ErrmsgOutOfSync) TypeName() string {
	return "ErrmsgOutOfSync"
}

type ErrmsgUndefinedError struct {
}

func (m *ErrmsgUndefinedError) TypeName() string {
	return "ErrmsgUndefinedError"
}

type OpaqueMessage map[string]*json.RawMessage

func (m *OpaqueMessage) TypeName() string {
	return "OpaqueMessage"
}

func (m *OpaqueMessage) SetID(id int) {
	bytesID := json.RawMessage(strconv.Itoa(id))
	(*m)["id"] = &bytesID
}

// some game messages

type MessageOrdinary struct {
	MessageIDEmbed
	Key string `json:"key" validate:"nonzero"`
}

func (m *MessageOrdinary) TypeName() string {
	return "MessageOrdinary"
}

type MessageJustMessage struct {
	MessageIDEmbed
	Text string `json:"text" validate:"nonzero"`
}

func (m *MessageJustMessage) TypeName() string {
	return "MessageJustMessage"
}

type MessageMouseClick struct {
	MessageIDEmbed
	Object *int `json:"obj" validate:"nonzero"`
}

func (m *MessageMouseClick) TypeName() string {
	return "MessageMouseClick"
}

type MessageOOC struct {
	MessageIDEmbed
	Login string `json:"login" validate:"nonzero"`
	Text  string `json:"text" validate:"nonzero"`
}

func (m *MessageOOC) TypeName() string {
	return "MessageOOC"
}

type MessagePing struct {
	MessageIDEmbed
	PingID string `json:"ping_id" validate:"nonzero"`
}

func (m *MessagePing) TypeName() string {
	return "MessagePing"
}
