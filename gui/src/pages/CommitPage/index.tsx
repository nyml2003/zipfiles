import React from 'react';
import CommitTable from './CommitTable';

const CommitPage: React.FC = () => {
  return (
    <div className='p-4 flex-1 flex flex-col'>
      <CommitTable />
    </div>
  );
};

export default CommitPage;
