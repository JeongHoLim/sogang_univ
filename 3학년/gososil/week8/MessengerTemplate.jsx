import React, { useState } from "react";
import styled from "styled-components";
import TextField from "@material-ui/core/TextField";

// Sending Message 부분
function MessenagerTemplate({getChatLog}) {
  const [ MsgState, setMsgState ] = useState({ message: "", username: ""})

  const onMsgChange = (e) => {
		setMsgState({ ...MsgState, [e.target.name]: e.target.value })
	}

  // 메시지 입력 창에서 빈 경우에는 ‘메시지를 입력하세요‘라는 내용의 경고 창 출력 후 리턴(alert 함수 사용)
  // 사용자의 이름 입력 창이 빈 경우에는 사용자 이름을 ‘이름없음’으로 저장
  // MsgState의 속성(키)들의 값을 빈 문자열로 초기화
	const onMsgSubmit = (e) => {
    
    e.preventDefault();
    // 8주차 실습 구현
    
    if(!MsgState.message){
      alert("메시지를 입력하세요")
      return;
    }
    else if(!MsgState.username)
      MsgState.username = "이름없음";

    getChatLog(MsgState.username,MsgState.message)
    setMsgState({message:"",username: ""})
  }

  return (
      <Form onSubmit={onMsgSubmit}>
				{/* 8주차 실습 구현 */
        <div>
          <Title>Sending Message</Title>
        <UserName>
          <TextField name = "username" label = "User Name" onChange = {(e) => onMsgChange(e)} value = {MsgState.username} ></TextField>
        </UserName>
        <MessageContents>
          <TextField name = "message" label = "Message" onChange = {(e) => onMsgChange(e)} value = {MsgState.message} ></TextField>
          </MessageContents>
        <Button>Send</Button>    
        </div>
      }
			</Form>
  );
}

// Sending Message 글씨 박스
const Title = styled.h1`
  margin: 10px;
  padding: 10px;
  // 해당 영역 모서리를 둥글게
  border-radius:10px;
  // 해당 영역의 배경색 변경
  background-color:rgba(216,184,99,1);
  // 해당 영역 안 텍스트 폰트색 변경
  color : rgba(255,255,255,1);
  // 8주차 실습 구현
`;

// 채팅 메시지 입력 form
const Form = styled.form`
  width: 500px;
  margin: 10px;
  padding: 20px;

  // 해당 영역 모서리를 둥글게
  border-radius: 10px;
  // 해당 영역 모서리에 그림자
  box-shadow: 0px 0px 10px rgba(0,0,0,.5);
  // 해당 영역의 배경색 변경
  background-color: rgba(255,253,228,1);
  // 8주차 실습 구현
`;

// username 표시
const UserName = styled.div`
  margin: 40px;
`;

// 채팅 메시지 부분
const MessageContents = styled.div`
  margin: 30px;
`;

// send 버튼
const Button = styled.button`
  margin-top: 60px;
  padding: 10px;
  // 해당 영역 모서리를 둥글게
  border-radius: 10px;
  // 해당 영역 모서리색 변경
  border-color : rgba(0,0,0,.05);
  // 해당 영역의 배경색 변경
  background-color : rgba(255,255,255,1);
  // 8주차 실습 구현
`;

export default MessenagerTemplate;