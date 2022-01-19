import { TextField } from "@material-ui/core";
import styled from "styled-components";
import React, { useState } from "react";

// 채팅 키워드 검색창 & 검색 버튼에 대한 컴포넌트 작성
function ChatSearch(onSearchKeySubmit) {

    const [keyword, setKeyState] = useState("")

    const onKeyChange = (e) => {
        setKeyState( e.target.value)
    }

    const onKeySubmit = (e) => {
  
        e.preventDefault()
        
        onSearchKeySubmit(keyword)
        setKeyState("")
        
    }

    return (
    
        <SearchContainer  onSubmit = {onKeySubmit}>
            <TextField name="keyword" onChange = {(e) => onKeyChange(e)} value = {keyword} variant="outlined" label="Keyword" />

            <Button>Search</Button>
       {/* ChatSearch 컴포넌트 구현 후 추가 */}
    
        </SearchContainer>
        
    );
  
  }


  const SearchContainer = styled.form`
  display : flex;
  justify-content : space-around;
  margin: 10px;
  padding: 20px;
`;
  const Button = styled.button`
  
  background-color: white;
  border-color: #c8a951;
  border-radius: 5px;
`;


  export default ChatSearch;